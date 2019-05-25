# CS 184: Computer Graphics and Imaging, Spring 2019

## Project 4: Cloth Simulation

### Zhiming Xu

## Overview

In this project, we mainly do physical simulation. The first part constructs the basic data structure for doing it. The second part deals with movement. We use a approximate method to substitute the computationally expensive numerical integration. In part three, we implement the collision between two different objects. The process is check whether the distance is below some threshold, if so, we apply some correction vector to the the vertex to move it to right position. For part four, the process is similar, but we should only move the loose vertices and keep the pinned ones. For the last part, we write shader programs that handle both vertex's position and color in OpenGL shading language.

## Part 1: Masses and springs

### Q1: Take some screenshots of scene/pinned2.json from a viewing angle where you can clearly see the cloth wireframe to show the structure of your point masses and springs.

- The screenshots are shown below.

  ![1](./images/q1-1.png)

  ![2](./images/q1-2.png)

  ![3](./images/q1-3.png)

### Q2:  Show us what the wireframe looks like (1) without any shearing constraints, (2) with only shearing constraints, and (3) with all constraints

- Without any shearing constraints

  ![1](./images/q1-4.png)

- With only shearing constraints

  ![2](./images/q1-5.png)

- With all constraints

  ![3](./images/q1-6.png)

## Part 2: Simulation via numerical integration

### Q1: Experiment with some the parameters in the simulation. To do so, pause the simulation at the start with P, modify the values of interest, and then resume by pressing P again. You can also restart the simulation at any time from the cloth's starting position by pressing R

#### Describe the effects of changing the spring constant ks; how does the cloth behave from start to rest with a very low ks? A high ks?

- Tested on `pinned2.json`. $k_s=50,5000,50000$, respectively, other parameters remain default.

- For a small $k_s$, the cloth slowly falls down and  comes to rest after swinging. There are many folds lying on the arc between two pinned points. And the arc has a relatively larger radius due to more elastic deformation required to hang the cloth. The lower part of the cloth also has an arc. 

  ![small](./images/small-ks.png)

- For a medium $k_s$, the speed of falling is a little bit larger. The cloth also comes to rest at the end. The arc between two pinned points seem very realistic and the folds extend to the lower part of the cloth.

  ![medium](./images/medium-ks.png)

- For a large $k_s$, the speed of falling again accelerates a little. The cloth comes to rest with no swinging at all. The arc between two pinned points has larger radius. And no folds can be seen below the arc.

  ![large](./images/large-ks.png)

#### What about for density?

- Tested on `pinned2.json`, $\rho=1.5,15,150$, respectively, other parameters remain default.

- For a small $\rho$, the cloth falls slowly and comes to rest after some swinging in lower part of it. The arc between two pinned points has a very large radius due to low weight of the cloth. There is only one fold on it. Alike above when $k_s$ is large.

  ![small](./images/small-rho.png)

- For a medium $\rho$, the cloth falls a little bit faster and comes to rest with little or no swinging. The folds extend to the lower part of it. The radius of the arc between two pinned points become smaller due to the increasing weight.

  ![medium](./images/medium-rho.png)

- For a large $\rho​$, the cloth falls again a little bit faster and comes to rest without any swinging. The arc between two pinned points become even smaller with multiple folds along with it. The lower part of the cloth also has an arc due to elastic deformation caused by large weight. (Image pull the cloth in the middle of the lower edge with a large force)

  ![large](./images/large-rho.png)

#### What about for damping?

- Tested on `pinned2.json`. $d=0.1,0.2,0.8$, respectively, other parameters remain default.

- For a small $d$, the cloth falls fast and comes to rest after some swinging since a small $d$  means less energy loss during movement.

  ![small-d](./images/small-d.png)

- For a medium $d$, the cloth falls a little bit more slowly and comes to rest after little to no swinging since now the energy runs out more quickly.

  ![medium](./images/medium-d.png)

- For a large $d​$, the cloth falls at a uniform slow speed at the beginning, and gradually loses speed as it becomes vertical to the ground. It takes a very long time for the cloth to rest after an extremely slow movement at the end.

  ![large-d](./images/large-d.png) 

#### For each of the above, observe any noticeable differences in the cloth compared to the default parameters and show us some screenshots of those interesting differences and describe when they occur

### Q2: Show us a screenshot of your shaded cloth from scene/pinned4.json in its final resting state

- The images are shown below, the first one uses default parameters, the second one changes $rho$ to 150, $x$ to 4, $y$ to 9.8, and $z=9.8$.

  ![default](./images/default-4.png)

  ![customized](./images/customized-4.png)

## Part 3: Handling collisions with other objects

### Q1: Show us screenshots of your shaded cloth from scene/sphere.json in its final resting state on the sphere using the default ks = 5000 as well as with ks = 500 and ks = 50000. Describe the differences in the results

- The images are shown below, $k_s=500, 5000, 50000$, respectively, other parameters remain default.

  ![500](./images/k500.png)

  ![5000](./images/k5000.png)

  ![50000](./images/k50000.png)

- With $k_s$ becomes larger, the elastic deformation of the cloth becomes less significant. And the folds of the cloth become fewer. The reason is that, by increasing $k_s$, the cloth is more and more "solid". The same gravity now causes less elastic deformation. So It's less likely to appear soft and smooth.

### Q2: Show us a screenshot of your shaded cloth lying peacefully at rest on the plane. If you haven't by now, feel free to express your colorful creativity with the cloth! (You will need to complete the shaders portion first to show custom colors.)

- The image is shown below.

  ![plane](./images/plane.png)

## Part 4: Handling self-collisions

### Q1: Show us at least 3 screenshots that document how your cloth falls and folds on itself, starting with an early, initial self-collision and ending with the cloth at a more restful state (even if it is still slightly bouncy on the ground)

- The images are shown below, in the beginning, middle and nearly at rest, respectively.

  ![initial](./images/initial.png)

  ![middle](./images/middle.png)

  ![rest](./images/rest.png)

### Q2: Vary the `density` as well as `ks` and describe with words and screenshots how they affect the behavior of the cloth as it falls on itself

#### Change density

- Tested on `selfCollision.json`, with $\rho=1.5,150,150$, respectively, other parameters remain default.

  ![small](./images/fold-small-rho.png)

  ![medium](./images/fold-medium-rho.png)

  ![large](./images/fold-large-rho.png)

- For a small $\rho$, the cloth is more fluffy, while for a large $\rho$, it is more compact due to the increasing weight.

#### Change $k_s$

- Tested on `selfCollision.json`, with $k_s=500,5000,5000$, respectively, other parameters remain default.

  ![small](./images/fold-small-ks.png)

  ![medium](./images/fold-medium-ks.png)

  ![large](./images/fold-large-ks.png)

- For a small $k_s$, the cloth is more compact, while for a large $k_s$, it is more fluffy. Since for a large $k_s$, it requires more force to cause elastic deformation.

## Part 5: Shader

### Q1: Explain in your own words what is a shader program and how vertex and fragment shaders work together to create lighting and material effects

- Shader is a kind of graphics program that controls the behavior of a vertex's position and color with vectors and matrices. The vertex shader mainly computes the position, normal and other properties related to space. While the fragment shader mainly computes which color a vertex should take on.

### Q2: Explain the Blinn-Phong shading model in your own words. Show a screenshot of your Blinn-Phong shader outputting only the ambient component, a screen shot only outputting the diffuse component, a screen shot only outputting the specular component, and one using the entire Blinn-Phong model

- The light effect in Blinn-Phong model consists of three parts. One is a uniform ambient light that cast on every vertex in the scene. Another is the diffuse light, which models the diffused light and is independent of the viewing direction. It obeys Lambertian law, calculated with $k_d(I/r^2)\max(\vec{n}\cdot\vec{l}, 0)$. The last part of it is specular light, which models the reflected light and is related to how close the viewing direction is to the reflecting direction, calculated with $k_s(I/r^2)\max(\vec{n}\cdot normalize(\vec{l}+\vec{v}))$.

- This image is lit with only ambient component.

  ![ambient](./images/ambient.png)

- This image is lit with only diffused component.

  ![diffuse](./images/diffuse.png)

- This image is lit with only specular component.

  ![specular](./images/specular.png)

- This image is lit with all three components.

  ![full](./images/full.png)

### Q3: Show a screenshot of your texture mapping shader using your own custom texture by modifying the textures in `/textures/`

- The image is shown below, the picture of seal is from assignment 1.

  ![seal](./images/seal.png)

### Q4: Show a screenshot of bump mapping on the cloth and on the sphere. Show a screenshot of displacement mapping on the sphere. Use the same texture for both renders. Compare the two approaches and resulting renders in your own words. Compare how your the two shaders react to the sphere by changing the sphere mesh's coarseness by using -o 16 -a 16 and then -o 128 -a 128

- Bump mapping on cloth and sphere.

  ![cloth](./images/bump-cloth.png)

  ![sphere](./images/bump-sphere.png)

- Displacement mapping on cloth and sphere.

  ![dis-cloth](./images/dis-cloth.png)

  ![dis-sphere](./images/dis-sphere.png)

- Displacement mapping modify the position of vertices and their normals in order to reflect the height map and to be consistent with the new geometry, which is not taken into account by bump mapping.

- The mapping effect of `-o 16 -a 16` is much coarser than that of `-o 128 -a 128`. We can easily see the differences in specular reflections, and edges of the textures and objects. The comparison is shown below. The first one is rendered with `16`, we can see the sphere has aliasing and the changes of lighting are not smooth. While in the second images, the sphere is round and lighting effect changes smoothly. *Note*: Due to the performance of my laptop, I'm not able to move the camera to the same angle in the second images. Since it will die shortly after applying texture mapping (due to insufficient memory perhaps).

  ![16](./images/16.png)

  ![128](./images/128.png)

### Q5: Show a screenshot of your mirror shader on the cloth and on the sphere

- The images are shown below.

  ![mirror-cloth](./images/mirror-cloth.png)

  ![mirror-sphere](./images/mirror-sphere.png)

### Q6: Custom shader

- I add mirror shading and bump shading together. Now the sphere has both environment map, texture and lighting effect on it. The comparison between it and mirror is shown below.

- Custom shader.

  ![custom](./images/custom.png)

- Mirror shader.

  ![mirror](./images/mirror.png)