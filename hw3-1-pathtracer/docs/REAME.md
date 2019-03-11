# CS 184: Computer Graphics and Imaging, Spring 2019

## Project 3-1: Pathtracer

### Zhiming Xu

## Overview

### Part 1: Ray Generation and Scene Intersection

#### Q1: Walk through the ray generation and primitive intersection parts of the rendering pipeline

- To generate rays, we need fill in the function `Camera::generate_ray` , which takes in two `double` as parameters. The ray formula is given by $\vec{r}=\vec{o}+t \cdot\vec{d}$. Where $\vec{o}$ is a vector representing its origin, $\vec{t}$ is a scalar representing the traveling time along its direction $\vec{d}$. The origin of the ray is the camera's location in the *world* space. The direction is determined by random sampling on the image plane. Time $t$ determines when the ray intersects with primitives in the world space. We need to transform the parameters `x, y`, ranging between $[0, 1]$, of `generat_ray`  into a point in the image plane. So we want the bottom left point to have coordinate $(0, 0)$ and top right point to have coordinate $(1, 1)​$ on the plane. Hence the corresponding `x, y` can be calculated with:

  ```
  X = bottom_left.x + (top_right.x - bottom_left.x) * x
  Y = bottom_left.y + (top_right.y - bottom_left.y) * y
  ```

  Besides, the coordinates of `bottom_left` and `top_right ` can be calculated with vertical and horizontal FoVs, as shown in the figures below.

  ![overall](./part1/1.jpg)

  ![separate](./part1/2.jpg) 

  The camera looks along $z$ axis reversely, and the image plane is of unit length in front of the camera. This will be the ray in camera space. Multiply it by `c2w` matrix will transform it into world space. For each pixel, If `ns_aa` is greater than $1$, we are going to randomly generate `ns_aa` rays pointing to different locations on the image plane and take the average of them, and then cast this average to the pixel. If `ns_aa` is $1$, we just cast a single ray to the center of the pixel.

#### Q2: Explain the triangle intersection algorithm you implemented in your own words

- Möller-Trumbore algorithm is used to detect ray-triangle intersection. The intersect point is given by $$\vec{P}=(1-b_1-b_2)\vec{P_0}+b_1\cdot\vec{P_1}+b_2\cdot\vec{P_2}$$, where $\vec{P_1}$, $\vec{P_2}$, and $\vec{P_3}$ are endpoints of the triangle, and $(1-b_1-b_2)$, $b_1$, and $b_2$ are barycentric coordinates of $\vec{P}$. By ray equation, we also have $$\vec{P}=\vec{O}+t\cdot\vec{D}$$, where $\vec{O}$ is the origin of ray, and $\vec{D}$ is its direction. Using this two equations, we can compute $t, b_1, b_2$ with Crammer's rule. Concretely, it is:

  $$\begin{pmatrix}t\\b_1\\b_2\end{pmatrix}=\cfrac{1}{\vec{S_1}\cdot\vec{E_1}}\begin{pmatrix}\vec{S_2}\cdot\vec{E_2}\\\vec{S_1}\cdot{S}\\\vec{S_2}\cdot\vec{D}\end{pmatrix}$$, where $$\begin{matrix}\vec{E_1}=\vec{P_1}-\vec{P_0}\\\vec{E_1}=\vec{P_2}-\vec{P_0}\\\vec{S}=\vec{O}-\vec{P_0}\\\vec{S_1}=\vec{D}\times\vec{E_2}\\\vec{S_2}=\vec{S}\times\vec{E_1}\end{matrix}$$

  After obtaining $t, b_1, b_2​$, we need to verify if it is really an intersection point, i.e., $t\in[ray.t_{min}, ray.t_{max}]​$ so that the intersection happens within the life time of the ray, and $0<b_1<1, 0<b_2<1​$ and $0<1-b_1-b_2<0​$ so that the intersection happens within the triangle. If we have found an intersection, we need to set the rays maximum $t_{max}​$ to the current $t​$ calculated above, because once an intersection happens at time $t​$, it can not happen in future because it is blocked by this triangle.

#### Q3: Show the normal shading for a few small *dae* files

- As shown in figures below.

  ![3](./part1/3.png)

  ![4](./part1/4.png)

  ![5](./part1/5.png)

### Part 2: Bounding Volume Hierarchy

#### Q1: Walk through your BVH construction algorithm. Explain the heuristic you chose for picking the splitting point

- BVH is a square box structure allows us to test ray intersection with axis-aligned planes rather then every single primitive in the scene. First, I pick the dimension with the largest extent to split the object, i.e., $\text{argmax}_{dim}extend.dim$. For simplicity, suppose we find $extend.x$ is the largest and $x$ dimension here. I then split the primitives into two `vector<Primitive*>`, `l` and `r` separately. The primitive with a centroid less then middle point of `centroid_box` goes to `l` and otherwise `r`. call `construct_bvh` on the left and right vector of primitives, `l`, and `r` recursively to continue splitting up the primitives, until the size of `prims` is less than the `max_leaf_size`, when the primitives are all stored in the `node->prims`. The procedure can be summarized as follows.

  ```c++
  construct_bvh(prims, max_leaf_size)
  if prims.size() exceeds max_leaf_size
  	use heuristic to split prims to two parts, prims_l and prims_r
  	construct_bvh(prims_l, max_leaf_size)
  	construct_bvh(prims_r, max_leaf_size)
  else
  	store prims in leaf node 
  ```

#### Q2: Walk through your BVH intersection algorithm

- We use the bounding box to test if a ray intersects the primitive more efficiently. We need to calculate the first and the last time our ray intersect with the *largest* box, or the extreme value along one dimension. For simplicity, the figure below shows the situation in 2D plane. 

  ![intersection](./part2/1.jpg)

  After calculating $t_{xmin}, t_{ymin},\cdots, t_{ymax}, t_{zmaz}$, we are going to obtain the *minimum* of $max$'s, $t_{min}$ and *maximum* of $min$'s, $t_{max}$ because they are exactly the times when our ray hits and leaves the bounding box. In the end, we need to check whether the intersection happens during the life time of our ray and hitting occurs before leaving, i.e., $t_{min},t_{max}\in[ray.min_t, ray.max_t]$ and $t_{min}<t_{max}$. We avoid checking whether the ray hits every primitive in the scene, instead we check whether it hits the primitives' bounding boxes. If it does intersect with bounding box, we then check if the box is a leaf node. If so, we just need to check every primitives in the box and see the ray hits any of them. If the box is not a leaf node, we traverse the left and right children by calling `BVHAccel::intersect(ray, node)` function recursively. The procedure can be summarized as follows.

  ```c++
  intersect(ray, node)
  if ray doest not intersect with node's bounding box
  	return false
  else
  	if node is leaf
  		check if ray intersect with any primitives in leaf
  		return result
  	else
  		intersect_left = intersect(ray, node->l)
  		intersect_right = intersect(ray, node->r)
  		return intersect_left || intersect_right
  ```

#### Q3: Show the normal shading for a few large dae files that you couldn't render without the acceleration structure

- These larger *dae* files unable to render without BVH before can be rendered easily now. Some of them are shown below.

  ![3](./part2/3.png)

  ![4](./part2/4.png)

  ![5](./part2/2.png)

#### Q4: Perform some rendering speed experiments on the scenes, and present your results and a 1-paragraph analysis

- I compared the rendering speed of 3 *.dae* files: Dragon_glossy, Maxplanck and cow. The speed increases dramatically for complicated scenes which has a great number of primitives. The time consumption before and after using bounding volume box are listed below

  - Dragon_glossy: from $596.2327$s to $0.0756$s, from average $33338.900$ intersections per ray to $6.380​$ intersections per ray.

    ![without](./part2/5.jpg)

    ![with](./part2/6.png)

  - Maxplanck: from $189.1278$s to $0.1111$s, from average $12244.543$ intersections per ray to $8.933​$ intersections per ray.

    ![without](./part2/11.png)

    ![with](./part2/10.png)

  - Cow: from $6.9462​$s to $0.1305​$s, from average $818.453​$ intersections per ray to $12.288​$ intersections per ray, the acceleration is not as significant as the previous two scenes.

    ![without](./part2/9.png)

    ![with](./part2/8.png)

### Part 3: Direct Illumination

#### Q1: Walk through both implementations of the direct lighting function

- For uniform hemisphere sampling, we sample the incident ray at a hit point randomly since we don't know the position of the light source. If the ray intersects with some objects in the scene, we calculate how much light is emitting from that intersection point and scale it to get its contribution to our primary ray (`L_out`, output ray from hit point). If the incident ray doesn't intersect with any object, we discard this sample.
- For importance sampling, we create a shadow ray whose origin is at the hit point (for implementation simplicity, we add a tiny offset, `EPS_D` to avoid frequently intersecting the same triangle at the same point again and again due to floating point imprecision) to see if it intersect with any objects before it reaches the location of the light currently is sampling. If a light is delta light, all the samples fall on the same location. So only one sample is taken. If it is not a delta light, the number of samples are set in the command line with parameter `-l <INT>`. If the sampled ray intersects with an object, we discard this sample. If it doesn't intersect with any object, we calculate the `bsdf` value, multiplied by `w_in.z`  and then divided by probability, and add the result to the sum of the samples. For each light, we take average of all the samples that don't intersect with any object and add them to the final output ray, `L_out`.

#### Q2: Show some images rendered with both implementations of the direct lighting function

- The images below are rendered with hemisphere illumination. With parameters `-s 64 -l 32 -m 6 -H`

  ![hem1](./part3/hem1.png)

  ![hem2](./part3/hem2.png)

- The image below is rendered with importance illumination. With parameters `-s 64 -l 32 -m 6`

  ![imp1](./part3/imp1.png)

  ![imp2](./part3/imp2.png)

#### Q3: Focus on one particular scene (with at least one area light) and compare the noise levels in soft shadows when rendering with 1, 4, 16, and 64 light rays (the `-l` flag) and 1 sample per pixel (the `-s` flag) using light sampling, not uniform hemisphere sampling

- These images are shown below, `l`  flag ranging from `16`  to `64`, `s` remains `1`.

  ![l16](./part3/l1.png)

  ![l16](./part3/l4.png)

  ![l32](./part3/l16.png)

  ![l64](./part3/l64.png)

#### Q4: Compare the results between using uniform hemisphere sampling and lighting sampling, including a 1-paragraph analysis

- Uniform sampling takes higher `samples_per_pixel` and higher `ray_per_pixel` to converge than importance sampling, otherwise it will take a lot of time to converge. However, lighting sampling has little noise even if with low `samples_per_pixel` and low `ray_per_pixel`. Using uniform hemisphere sampling and lighting sampling on Cbunny.dae. With the same number of samples, hemisphere sampling is much more noisy than lighting sampling. The comparison is shown below.

  - Hemisphere, `-s 1 -l 4`

    ![hems1l1](./part3/hems1l4.png)

  - Hemisphere, `-s 1 -l 32`

    ![hems1l32](./part3/hems1l32.png)

  - Hemisphere, `-s 1 -l 128`

    ![hems1l128](./part3/hems1l128.png)

  - Lighting, `-s 1 -l 4`

    ![ligs1l4](./part3/ligs1l4.png)

  - Lighting, `-s 1 -l 32`

    ![ligs1l4](./part3/ligs1l32.png)

  - Lighting, `-s 1 -l 128`

    ![ligs1l4](./part3/ligs1l128.png)

### Part 4: Global Illumination

#### Q1: Walk through your implementation of the indirect lighting function

- The function `est_radiance_global_illumination` is called to get an estimate of the total radiance with global illumination arriving at a point from a particular direction, e.g., on the image plane and going towards the image's center of projection. The function `zero_bounce_radiance` should return the spectrum that results from no bounces of light, which is simply the light emitted by the given point along its outgoing direction. The pixel will be black unless it lies on a light. The function `one_bounce_radiance` is the direct illumination that we have implemented in Part 3. We only need to call our direct lighting function according to the indicator `direct_illumination_sample`. The function `at_least_one_bounce_radiance` calls `one_bounce_radiance`, and then recursively call itself to estimate the higher bounces. This recursive call takes one random sample from the `bsdf` at the hit point, trace a ray in that sample direction, and recursively call itself on the new hit point. We set a parameter `max_ray_depth` to control the maximum levels of recursions. However, with some probability, rays might still go into deeper recursions to improve performance, and we use Russian Roulette to terminate the rays. Pictures shown below.

#### Q2: Show some images rendered with global (direct and indirect) illumination. Use 1024 samples per pixel

- The images are shown below, rendered with parameters `-s 1024 -l 1 -m 3`, the latter two are used to save time.

  ![global1](./part4/global1.png)

  ![global2](./part4/global2.png)

#### Q3: Pick one scene and compare rendered views first with only *direct* illumination, then only *indirect* illumination. Use 1024 samples per pixel

#### Q4: For CBbunny.dae, compare rendered views with `max_ray_depth` equal to 0, 1, 2, 3, and 100 (the -m flag). Use 1024 samples per pixel

- The images below are rendered with `-s 1024 -l 1 -m [0, 1, 2, 3, 100`, respectively. As the max ray depth increases, there are more areas that are lit by indirect illumination. Thus the dark part of the images are disappearing gradually.

  ![m0](./part4/m0.png)

  ![m1](./part4/m1.png)

  ![m2](./part4/m2.png)

  ![m3](./part4/m3.png)

  ![m100](./part4/m100.png)

#### Q4: Pick one scene and compare rendered views with various `sample-per-pixel` rates, including at least 1, 2, 4, 8, 16, 64, and 1024. Use 4 light rays

- The images below are rendered with `-s [1, 2, 4, 8, 16, 64, 1024] -l 4 -m 6`, respectively. With the number of samples per pixel increasing, the image becomes clearer, more samples help to reduce noises, as in anti-aliasing. 

  ![s1](./part4/s1.png)

  ![s2](./part4/s2.png)

  ![s4](./part4/s4.png)

  ![s8](./part4/s8.png)

  ![s16](./part4/s16.png)

  ![s64](./part4/s64.png)

  ![s1024](./part4/s1024.png)

### Part 5: Adaptive Sampling