# CS 184: Computer Graphics and Imaging, Spring 2019

## Project 3-2: Pathtracer2

### Zhiming Xu

## Overview

For this project, I added more features to ray tracing in previous part to create more realistic scenes. First, I implemented more materials, like mirror and glass, which will cast rays in a particular direction and generate a different BSDF depending on where the light comes from. Then I implemented microfacet materials which can represent specular surfaces with a wide range of reflective properties. Next, I implemented environment lighting starting from an infinite distance. In the end, I implemented thin lens camera effects, which allows me to simulate the behavior of a real-world camera by modifying focal length and lens radius.

## Part 1: Mirror and Glass

### Q1: Multibounces effects

- The images of scene `CBspheres.dae` is shown below, rendered with `-s 64 -l 4 -m i`, where `i=0,1,2,3,4,5,100`, respectively.

  ![m0](./images/CBsphere0.png)

  ![m0](./images/CBsphere1.png)

  ![m0](./images/CBsphere2.png)

  ![m0](./images/CBsphere3.png)

  ![m0](./images/CBsphere4.png)

  ![m0](./images/CBsphere5.png)

  ![m0](./images/CBsphere100.png)

- The particular effects caused by new bounces

  - From 0 to 1: In zero bounce, only pixels lying on the light are visible, because we only count emission light. When `m` set to `1`, the pixels directly under light are visible. Thus the floor and walls are bright.
  - From 1 to 2: In one bounce, the reflections are not taken into account since the rays can not hit camera in just on bounce. So in one bounce, the spheres are both dark. Then in two bounces, the spheres' reflected rays hit camera in the second bounce, and they become bright. Besides, the ceiling also become bright because of diffused light from other surfaces.
  - From 3 to 4: In the fourth bounce, refraction shows up, because transition from one material to another is considered one bounce. So it takes four bounces for light to travel from light source through glass sphere to the ground.
  - From 4 to 5: Light spots on wall do not show until 5th bounce because light need to first refract to the wall (4 bounces), then diffused into the camera. 100 bounces produces no significant difference compared to 5 bounces.

## Part 2: Microfacet materials

### Q1: Show a sequence of 4 images of scene `CBdragon_microfacet_au.dae` rendered with *α* set to 0.005, 0.05, 0.25 and 0.5. Describe the differences between different images

- The images are shown below, with $\alpha=0.5, 0.25, 0.05, 0.005$, respectively.

  ![CBdragon](./images/CBdragon5.png)

  ![CBdragon](./images/CBdragon25.png)

  ![CBdragon](./images/CBdragon05.png)

  ![CBdragon](./images/CBdragon005.png)

- With alpha increasing, the surface of the dragon becomes less reflective. When $\alpha=0.005$, the dragon is  almost fully reflective, and its colors are influenced by the wall. Concretely, its surface is red towards the red wall, and blue towards the blue wall on top of Au's original color, golden. Besides, this scene has much noise. However, the scene becomes even more noise when $\alpha=0.05$, which is reflecting fewer rays from walls and more from the light source. When $\alpha = 0.25$, we can still see the walls reflected in the neck and back of the dragon. But now it is mostly reflecting rays from the light source. When $\alpha = 0.5$, the surface is nearly all about diffusion, which becomes bright, has little noise, and shows the original color of Au, golden.

### Q2: Show two images of scene `CBbunny_microfacet_cu.dae` rendered using cosine hemisphere sampling (default) and your importance sampling. Briefly discuss their difference

- The images are shown below, first hemisphere and second importance, the other parameters are `-s 64 -l 1 -m 5`.

  ![bunny_hemi](./images/CBbunny_hemi.png)

  ![bunny_imp](./images/CBbunny_imp.png)

- As we can tell from the images, cosine hemisphere sampling generates much more noise, because it samples from a random direction instead of sampling towards the light as in importance sampling. Besides, there are many black spots on the cosine hemisphere sampled bunny, especially around the edges. The importance sampled bunny is more realistic and has almost no noise on itself. The noise concentrate on the rest of the scene. Since the $\alpha$ of the bunny is $0.1$, the reason why noise come about is similar to the previous question on dragon with $\alpha=0.05$, i.e., it reflects more rays from the light source and less from the wall, and produce the noise.

### Q3: Show at least one image with some other conductor material

- This is a dragon made of Ti, which shows typically silver surface of metal.

  ![dragon_ti](./images/CBdragon_Ti.png)

- They are spheres made of Cs and Na, respectively, with Cs on the left and Na on the right. We can observe that Cs looks a little bit golden as introduced by chemists while Na is silver, like typical metal.

  ![spheres_sr_na](./images/CBsphere_cs_na.png)

## Part 3: Environment light

### Q1: In a few sentences, explain the ideas behind environment lighting

- Environment lighting adds light to the scene as if it came from a sphere surrounding the scene. The light source is supposed to be from an infinitely far away distance and is representative of realistic lighting environments in the real world.

### Q2: Show the *probability_debug.png* file for the *.exr* file you are using

- This is the probability for `grace.exr`

  ![grace](./images/grace_prob.png)

### Q3: Use the `bunny_unlit.dae` scene and your environment map *.exr* file and render two pictures, one with uniform sampling and one with importance sampling. Compare noise levels

- The images below uses `grace.exr` and the other parameters are `-s 64 -l 4 -m 5`, first hemisphere and second importance.

  ![hemi](./images/bunny_hemi.png)

  ![imp](./images/bunny_imp.png)

- With `bunny_unlit`, the difference between hemisphere sampling and importance sampling is extremely small, almost negligible. We can only observe that the shadows of importance sampled bunny is a little bit darker, especially in the legs.

### Q4: Use the `bunny_microfacet_cu_unlit.dae` and your environment map *.exr* file and render two pictures, one with uniform sampling and one with importance sampling. Compare noise levels

- The images below uses `field.exr` and the other parameters are `-s 64 -l 4 -m 5`, first hemisphere and second importance.

  ![hemi](./images/bunny_cu_hemi.png)

  ![imp](./images/bunny_cu_imp.png)

-  The importance sampled bunny has much less noise. It only reflects noise from the base. But the hemisphere sampled bunny is very noisy, especially near edges. The bunny and its base also have some white spots with hemisphere sampling, which are not present in importance sampling.

## Part 4: Depth of field

### Q1: In a few sentences, explain the differences between a pinhole camera model and a thin-lens camera model

- A **pinhole camera** is a simple camera without a lens but with a tiny aperture, which is the pinhole on one side of a black box. Light from a scene passes through the aperture and projects an inverted image on the opposite side of the box.
- A **thin lens camera**, however, has a thin lens whose thickness is negligible. Its property is expressed by thin lens equation: $\frac{1}{f}=\frac{1}{z_i}+\frac{1}{z_o}$. So in order to focus on the object, we need to move the sensor relative to the lens. For $z_i<z_o$, the image is smaller than the objective. For $z_i=z_o$, the image is of the same size as object. And for $z_i>z_o$, the image is larger than object. So we can not focus on objects that have $z_o\le f$.

### Q2: Show a "focus stack" where you focus at 4 visibly different depths through a scene

- The focal lengths are set to `3.5, 4, 4.5, 5`, respectively. The lens radii are fixed to `0.2`. Other parameters are `-s 64 -l 1 -m 5`.

  ![3.5](./images/CBsphere_d3.5.png)

  ![4](./images/CBsphere_d4.png)

  ![4.5](./images/CBsphere_d4.5.png)

  ![5](./images/CBsphere_d5.png)

### Q3: Show a sequence of 4 pictures with visibly different aperture sizes, all focused at the same point in a scene.

- The lens radii are set to `0.1, 0.2, 0.4, 0.8`, respectively. The focal lengths are fixed to `4.5`. Other parameters are `-s 64 -l 1 -m 5`.

  ![0.1](./images/CBsphere_b0.1.png)

  ![0.2](./images/CBsphere_b0.2.png)

  ![0.4](./images/CBsphere_b0.4.png)

  ![0.8](./images/CBsphere_b0.8.png)

