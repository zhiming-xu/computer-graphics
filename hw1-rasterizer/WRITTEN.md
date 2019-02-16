# CS 184: Computer Graphics and Imaging, Spring 2019
## Project 1: Rasterizer
### Zhiming Xu, 3034485754
### Overview
What we mainly do in this project is draw an triangle. Concretely, we first determine whether a pixel on the screen belongs to a certain triangle by testing the pixel's relative position with regard to the triangle's edges and use supersampling to avoid aliasing (Part 1 and Part 2). Besides, we want to be able to change the triangles' shapes, where the homogeneous coordinates arise (Part 3). We then need to assign appropriate colors for each pixel. So we introduce a method to convert Cartesian coordinates to barycentric ones, so that we can extract a particular color from a color triangle (in structure `ColorTri`) by them. (Part 4 and Part 5). In the end, we also want to add textures to our triangles, therefore, we make use of texture triangles ( in structure `TexTri`) to extract the color of the pixel (also with barycentric coordinates) we are sampling. 
## Section I: Rasterization
### Part 1: Rasterizing single-color triangles
- I think rasterizing a triangle is like coloring grids by sampling. Since in real world, the edges of a triangle are continuous, but on screen, they need to be converted to a finite number of grids. Therefore, when given the coordinates of endpoints of a triangle, we need to determine those vertices included in it, and give them correct colors so that they original triangle can be displayed on screen.
- My algorithm first find the min and max value of x's and y's. And only sample the grids within this range, i.e., sample all vertices included in the minimum bounding box of the triangle. Therefore, it is exactly the same as what's mentioned in the question.
- As is shown in the figure below. 
![Screenshot of test4. Obviously, the edges of cubic have many jaggies due to low sampling rate](./screenshot_2-9_15-57-56.png)
### Part 2: Antialiasing triangles
- Instead of just sampling once for each pixel, this time we need to sample muliple times for each pixel. 
  Concretely, we divide one pixel into `sample_per_side` subpixels per side, therefore, we now need to sample`sample_per_side * sample_per_side` times for one pixel. And average their values to derive its color. The reason why supersampling is useful is that it samples more pixels so that better determine to what degree a pixel belongs to
  a triangle instead of just deciding inside or not. I added subpixel sampling in previous pixel sampling. By supersampling, the pixels partially in a triangle are assigned an intermediate color between blank and full. Thus the edges look smoother.
- As are shown in the figures below.
  ![1x Sampling](./screenshot_2-9_16-30-25.png)![2x Supersampling](./screenshot_2-9_16-30-30.png)
  ![3x Sampling](./screenshot_2-9_16-30-34.png)![4x Supersampling](./screenshot_2-9_16-30-39.png)
### Part 3: Transforms
I modify the part that controlling the cubeman's arms and make it look like jumping down.

![Robot](screenshot_2-12_21-22-29.png)

## Section 2: Sampling
### Part 4: Barycentric coordinates
- I think barycentric coordinates is another way to represent the location of a point within a specific triangle.
- As is shown in the figure below.
![svg/basic/test7](./screenshot_2-10_20-39-25.png)
### Part 5: Pixel sampling
- Pixel sample is determine a pixel's relevant location (via barycentric coordinates) within a triangle and use the color in the same location of the texture to color this pixel. I implemented it in the following way:
    - `raterize_trianlge` has a parameter `tri`, which in this case is a pointer to `TexTri` structure, and has a virtual method `TexTri::color`.
    - We need to use this method to derive texture color, which takes in the sampled pixel's barycentric coordinates `p_bary` and `sp` which indicates sampling parameters as the only useful parameters. 
    - In `tri->color`, the weighted sum of texture triangle's endpoints' colors, i.e., `p0_col`, `p1_col`, and `p2_col`, is calculated and set as `sp.p_uv`.
    - `tri->tex->sample` is called to derive correct color by either *nearest* or *linear*, which returns the color of the nearest pixels of texture or the linear interpolation of the four nearest pixels of texture.
- Nearest: as this name suggests, substitute the sampled pixel with a texture pixel nearest to it.
- Linear: substitute the sampled pixel with the weighted linear sum of four texture pixels nearest to it.
- The four figures show: 1x nearest, 16x nearest, 1x linear, 16x linear, respectively
![1x nearest](./screenshot_2-10_22-0-25.png)![16x nearest](./screenshot_2-10_22-0-33.png)
![1x linear](./screenshot_2-10_22-0-48.png)![16x linear](./screenshot_2-10_22-0-41.png)
- I think if there is some thin part, linear method will perform better because nearest method is likely to ignore this part because it is substituted with a nearby pixel. 
### Part 6: Level sampling
- Level sampling is a method in addition to pixel sampling. In order to deal with images' different sizes, we can pick the most suitable texture to sample a pixel to get better results. I implemented it by completing `get_level` method and pass it to previous pixel sampling methods according to `sp.lsm`. For `L_NEAREST`, calculate image size and use the most similar mipmap to color the pixel. For `L_LINEAR`, use the two most similar mipmap's linear interpolation to color the pixel.

- I think if not room, supersampling looks quite satisfying, however, it will significantly increase time and space complexity (by `samples_per_side^2` times). And level sampling is especially good for zoomed images, like Mario shown below. Without level sampling, the lower part of him can't be shown due to its  size. Pixel sampling are also good at avoiding jaggies, we can see clearly from the comparison between the third and fourth pictures, where the jaggies are gone by `P_LINEAR`. And both of the latter two methods increase memory consumption and time complexity no more than $O(1)$, which are computationally cheap than supersampling.

- The comparison is shown below, they are `L_ZERO, P_NEAREST`, `L_ZERO, P_LINEAR`, `L_NEAREST, P_NEAREST`,  and `L_NEAREST, P_LINEAR`, respectively.

  ![LZPN](./screenshot_2-12_21-13-14.png)

  ![LZPL](./screenshot_2-12_21-13-16.png)

  ![LNPN](./screenshot_2-12_21-20-50.png)

  ![LNPL](screenshot_2-12_21-20-55.png) 