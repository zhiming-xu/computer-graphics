# Project 1: Rasterizer

<img src="https://cs184.eecs.berkeley.edu/cs184_sp16_content/article_images/3_1.jpg" width="800px" align="middle"/>

In this assignment you will implement a simple rasterizer, including features like supersampling, hierarchical transforms, and texture mapping with antialiasing. At the end, you'll have a functional vector graphics renderer that can take in modified SVG (Scalable Vector Graphics) files, which are widely used on the internet.

## Logistics

### Deadline
* Project 1 is due Tuesday, February 12th at 11:59pm. Both your code and write-up need to be turned in for your submission to be complete; assignments which are turned in after 11:59pm are a full day late -- there are no late minutes or late hours.


### Getting started

First, accept the assignment in your CS184 website profile, following the instructions from GitHub Classroom. Then, clone the generated repo. Make sure you clone _your repo_, and not the class skeleton.

    $ git clone --recurse-submodule <YOUR_PRIVATE_REPO>
    
Also ensure GitHub Pages is enabled for your assignment.

Please consult this article for more information on [how to build and submit assignments for CS 184](https://cs184.eecs.berkeley.edu/sp19/article/8/building-and-submitting-cs184-as).

As you go through the assignment, [refer to the write-up guidelines and deliverables section below](#rubric). **It is recommended that you accumulate deliverables into sections in your webpage write-up as you work through the project.** You may find it helpful to skim this section before beginning your work.

*Note: Do not squander all your hard work on this assignment by converting your png files into jpg or any other format!* Leave the screenshots as they are saved by the `'S'` key in the GUI, otherwise you will introduce artifacts that will ruin your rasterization efforts.

Finally, you may find the following resources helpful: 
**[C++ Guide](https://cs184.eecs.berkeley.edu/sp19/article/11/c-basics)** for some quick tips and tricks on getting started with C++. A slightly more detailed C++ guide can be [found here](https://github.com/Bryce-Summers/Writings/blob/master/Programming%20Guides/C_plus_plus_guide.pdf).
**[Images as Data](https://cs184.eecs.berkeley.edu/sp19/article/7/images-as-data)** on how images and colors are often represented in code.

<img src="https://cs184.eecs.berkeley.edu/cs184_sp16_content/article_images/3_7.jpg" width="800px" align="middle"/>

## Using the GUI

You can run the executable with the command

    ./draw [path to svg file/folder to render]
    
Example:
    
    ./draw ../svg/basic/test1.svg

A flower should show up on your screen (after you do some work in Part 1). After finishing Part 3, you will be able to change the viewpoint by dragging your mouse to pan around or scrolling to zoom in and out. Here are all the keyboard shortcuts available (some depend on you implementing various parts of the assignment):

|Key | Action|
|:-----:|------|
|`' '`  | return to original viewpoint|
|`'-'`  | decrease sample rate|
|`'='` | increase sample rate|
|`'Z'` | toggle the pixel inspector|
|`'P'` | switch between texture filtering methods on pixels|
|`'L'` | switch between texture filtering methods on mipmap levels|
|`'S'` | save a *png* screenshot in the current directory|
| `'1'-'9'`  | switch between svg files in the loaded directory|

The argument passed to `draw` can either be a single file or a directory containing multiple *svg* files, as in

    ./draw ../svg/basic/

If you load a directory with up to 9 files, you can switch between them using the number keys 1-9 on your keyboard.

## Project structure

The project has 7 parts, divided into 3 sections, worth a total of 100 possible points. Some require only a few lines of code, while others are more substantial.

**Section I: Rasterization (suggested completion checkpoint: Tuesday, Feb 5)**

* Part 1: Rasterizing single-color triangles (20 pts)
* Part 2: Antialiasing triangles (20 pts)
* Part 3: Transforms (10 pts)

**Section II: Sampling**

* Part 4: Barycentric coordinates (10 pts)
* Part 5: "Pixel sampling" for texture mapping (15 pts)
* Part 6: "Level sampling" with mipmaps for texture mapping (25 pts)

**Section III: (optional, possible extra credit) Art Competition**

* Part 7: Draw something interesting!

### Codebase

There is a fair amount of code in the CGL library, which we will be using for future assignments. The relevant header files for this assignment are *vector2D.h*, *matrix3x3.h*, *color.h*, and *renderer.h*.

Here is a very brief sketch of what happens when you launch `draw`: An `SVGParser` (in *svgparser.\**) reads in the input *svg* file(s), launches a OpenGL `Viewer` containing a `DrawRend` renderer, which enters an infinite loop and waits for input from the mouse and keyboard. `DrawRend` (*drawrend.\**) contains various callback functions hooked up to these events, but its main job happens inside the `DrawRend::redraw()` function. The high-level drawing work is done by the various `SVGElement` child classes (*svg.\**), which then pass their low-level point, line, and triangle rasterization data back to the three `DrawRend` rasterization functions.

Here are the files you will be modifying throughout the project:

1. *drawrend.cpp*, *drawrend.h*
2. *texture.cpp*
3. *transforms.cpp*
4. *svg.cpp*

In addition to modifying these, you will need to reference some of the other source and header files as you work through the project.

## Section I: Rasterization

### Part 1: Rasterizing single-color triangles (20 pts)
[**Relevant lecture: 2**](https://cs184.eecs.berkeley.edu/sp19/lecture/2/rasterization)

Triangle rasterization is a core function in the graphics pipeline to convert input triangles into framebuffer pixel values. In Part 1, you will implement basic triangle rasterization.

You may first want to read [this resource](https://cs184.eecs.berkeley.edu/sp19/article/7/images-as-data) on how colors and images are represented as data. 

First, fill in the `SampleBuffer::fill_color()` function in *drawrend.h*, which sets the color of a single sub-pixel sample. Each `SampleBuffer` instance represents a single pixel, and holds the many sub-pixel samples for this individual pixel in `std::vector<std::vector<PixelColorStorage>> sub_pixels`.

A `PixelColorStorage` is defined in *drawrend.h* as a vector of `unsigned char`, which you can think of as an array of `[r,g,b]` format. Thus, the `SampleBuffer` isa 2D array of `[r,g,b]`s. 

Sub-pixel sampling will be useful in Part 2, but in this step you should only sample once per pixel by calling `SampleBuffer::fill_pixel()` after you finish`SampleBuffer::fill_color()`.

Next, fill in the `DrawRend::rasterize_triangle(...)` function in *drawrend.cpp* using the methods discussed in lecture 2.

Notes:

 * `PixelColorStorage` stores each r, g, and b channel as uint8 from 0 to 255, while `Color` stores them as floats from 0.0 to 1.0. Pay attention to the different data types.
  * Remember to do point-in-triangle tests with the point exactly at the center of the pixel, not the corner. Your coordinates should be equal to some integer point plus (.5,.5).
  * You should not make any assumptions about the ordering of the given points (i.e. whether or not they are given in clockwise order). Check `svg/basic/test6.svg`.
  * For now, ignore the `Triangle *tri` input argument to the function. We will come back to this in Part 4.
  * You are encouraged but not required to implement the edge rules for samples lying exactly on an edge.
  * Make sure the performance of your algorithm is no worse than one that checks each sample within the bounding box of the triangle.

When finished, you should be able to render many more test files, including those with rectangles and polygons, since we have provided the code to break these up into triangles for you. In particular, *basic/test3.svg*, *basic/test4.svg*, and *basic/test5.svg* should all render correctly.

For convenience, here is a list of functions you will need to modify:

1. `SampleBuffer::fill_color` in *drawrend.h*
2. `DrawRend::rasterize_triangle` in *drawrend.cpp*


**Extra Credit (up to X points):** Make your triangle rasterizer super fast (e.g., by factoring redundant arithmetic operations out of loops, minimizing memory access, and not checking every sample in the bounding box). Write about the optimizations you used. Use `clock()` to get timing comparisons between your naive and speedy implementations.



### Part 2: Antialiasing triangles (20 pts)
[**Relevant lecture: 3**](https://cs184.eecs.berkeley.edu/sp19/lecture/3/antialiasing)

Use supersampling to antialias your triangles. The `sample_rate` parameter in `DrawRend` (adjusted using the `-` and `=` keys) tells you how many samples to use per pixel.

The image below shows how sampling four times per pixel produces a better result than just sampling once, since some of the supersampled pixels are partially covered and will yield a smoother edge.

<img src="https://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/3_1.jpg" width="500px" align="middle"/>

To do supersampling, each pixel is now divided into `sqrt(sample_rate) * sqrt(sample_rate)` sub-pixels. In other words, you still need to keep track of `height * width` pixels, but now each pixel has `sqrt(sample_rate) * sqrt(sample_rate)` sampled colors. You will need to do point-in-triangle tests at the center of each of these *sub-pixel* squares.

Unlike in Part 1, you will use the provided `SampleBuffer` class to store each sub-pixel sample by calling `SampleBuffer::fill_color()` directly instead of `SampleBuffer::fill_pixel()`. You will also need to update `SampleBuffer::get_pixel_color()` to average all of the sub-pixel samples to compute the final output color of that pixel (instead of simply discarding all but one sub-pixel's sample data). Your implementation will be called by `DrawRend::resolve()` in during the final rasterization steps.

Your triangle edges should be noticeably smoother when using > 1 sample per pixel! You can examine the differences closely using the pixel inspector (see controls listed above). Also note that, it may take several seconds to switch to a higher sampling rate.

For convenience, here is a list of functions you will need to modify:

1. `DrawRend::rasterize_triangle`
2. `SampleBuffer::get_pixel_color` in *drawrend.h*

**Extra Credit:** Implement an alternative sampling pattern, such as jittered or low-discrepancy sampling. Create comparison images showing the differences between grid supersampling and your new pattern. Try making a scene that contains aliasing artifacts when rendered using grid supersampling but not when using your pattern. You can also try to implement more efficient storage types for supersampled framebuffers, instead of using one samplebuffer per pixel.


### Part 3: Transforms (10 pts)
**Relevant lecture: TBD**
<!--[**Relevant lecture: 4**](https://cs184.org/lecture/transforms-1)-->

Implement the three transforms in the *transforms.cpp* file according to the [SVG spec](https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/transform). The matrices are 3x3 because they operate in homogeneous coordinates -- you can see how they will be used on instances of `Vector2D` by looking at the way the `*` operator is overloaded in the same file.

Once you've implemented these transforms, *svg/transforms/robot.svg* should render correctly, as follows:

<img src="https://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/3_.jpg" width="400px" align="middle"/>

For convenience, here is a list of functions in *transforms.cpp* you will need to modify:

1. `translate`
2. `scale`
3. `rotate`

**Extra Credit:** Add an extra feature to the GUI. For example, you could make two unused keys to rotate the viewport. Save an example image to demonstrate your feature, and write about how you modified the SVG to NDC and NDC to screen-space matrix stack to implement it.


## Section II: Sampling

### Part 4: Barycentric coordinates (10 pts)
**Relevant lecture: TBD**
<!--[**Relevant lecture: 6**](https://cs184.eecs.berkeley.edu/lecture/texture)-->

Familiarize yourself with the `ColorTri` struct in *svg.h*. Modify your implementation of `DrawRend::rasterize_triangle(...)` so that if a non-NULL `Triangle *tri` pointer is passed in, it computes barycentric coordinates of each sample hit and passes them to `tri->color(...)` to request the appropriate color. Note that the barycentric coordinates are stored with type `Vector3D`, so you should use `p_bary[i]` to store the barycentric coordinate corresponding to triangle vertex $P_i$ for $i=0,1,2$.

Implement the `ColorTri::color(...)` function in *svg.cpp* so that it interpolates the color at the point `p_bary`. This function is very simple: it does not need to make use of `p_dx_bary` or `p_dy_bary`, which are for texture mapping. Note that this `color()` function plays the role of a very primitive shader.

Once Part 4 is done, you should be able to see a color wheel in *svg/basic/test7.svg*.

For convenience, here is a list of functions you will need to modify:

1. `DrawRend::rasterize_triangle`
2. `ColorTri::color` in *svg.cpp*

<img src="https://cs184.eecs.berkeley.edu/cs184_sp16_content/article_images/3_4.jpg" width="800px" align="middle"/>


### Part 5: "Pixel sampling" for texture mapping (15 pts)
**Relevant lecture: TBD**
<!--[**Relevant lecture: 6**](https://cs184.eecs.berkeley.edu/lecture/texture)-->

Familiarize yourself with the `TexTri` struct in *svg.h*. This is the primitive that implements texture mapping. For each vertex, you are given corresponding *uv* coordinates that index into the `Texture` pointed to by `*tex`.

To implement texture mapping, `DrawRend::rasterize_triangle` should fill in the `psm` and `lsm` members of a `SampleParams` struct and pass it to `tri->color(...)`. Then `TexTri::color(...)` should fill in the correct *uv* coordinates in the `SampleParams` struct, and pass it on to `tex->sample(...)`. Then `Texture::sample(...)` should examine the `SampleParams` to determine the correct sampling scheme.

The GUI toggles `DrawRend`'s `PixelSampleMethod` variable `psm` using the `'P'` key. When `psm == P_NEAREST`, you should use nearest-pixel sampling, and when `psm == P_LINEAR`, you should use bilinear sampling.

For now, you can pass in dummy `Vector3D(0,0,0)` values for the `p_dx_bary` and `p_dy_bary` arguments to `tri->color`

For this part, just pass `0` for the `level` parameter of the `sample_nearest` and `sample_bilinear` functions.

Once Part 5 is done, you should be able to rasterize the *svg* files in *svg/texmap/*, which rely on texture maps.

Notes:

* `texels` are implemented as the typical [r,g,b,r,g,...b] format

For convenience, here is a list of functions you will need to modify:

1. `DrawRend::rasterize_triangle`
2. `TexTri::color`
3. `Texture::sample`
4. `Texture::sample_nearest`
5. `Texture::sample_bilinear`


### Part 6: "Level sampling" with mipmaps for texture mapping (25 pts)
**Relevant lecture: TBD**
<!--[**Relevant lecture: 6**](https://cs184.eecs.berkeley.edu/lecture/texture)-->

Finally, add support for sampling different `MipMap` levels. The GUI toggles `DrawRend`'s `LevelSampleMethod` variable `lsm` using the `'L'` key.

* When `lsm == L_ZERO`, you should sample from the zero-th `MipMap`, as in Part 5.
* When `lsm == L_NEAREST`, you should compute the nearest appropriate `MipMap` level using the one-pixel difference vectors `du` and `dv` and pass that level as a parameter to the nearest or bilinear sample function.
* When `lsm == L_LINEAR`, you shoould find the appropriate `MipMap` level. Then compute a weighted sum of using one sample from each of the adjacent `MipMap` levels.

Implement `Texture::get_level` as a helper function. You will need $(\frac{du}{dx}, \frac{dv}{dx})$ and $(\frac{du}{dy},\frac{dv}{dy})$ to calculate the correct `MipMap` level. In order to get these values corresponding to a point $p = (x,y)$ inside a triangle, you must:

1. calculate the barycentric coordinates of $(x+1,y)$ and $(x,y+1)$ in `rasterize_triangle`, passing these to `tri->color` as the variables `p_dx_bary` and `p_dy_bary`,
2. calculate the *uv* coordinates `sp.p_dx_uv` and `sp.p_dy_uv` inside `tri_color`,
3. calculate the difference vectors `sp.p_dx_uv - sp.p_uv` and `sp.p_dy_uv - sp.p_uv` inside `Texture::get_level`, and finally
4. scale up those difference vectors respectively by the width and height of the full-resolution texture image.

With these, you can proceed with the calculation from the lecture slides.

Notes:

* The `lsm` and `psm` variables can be set independently and interact independently. In other words, all combinations of `psm==[P_NEAREST, P_LINEAR] x lsm==[L_ZERO, L_NEAREST, L_LINEAR]` are valid.
* As a point of interest, when `lsm == L_LINEAR` and `psm == P_LINEAR`, this is known as trilinear sampling. 

For convenience, here is a list of functions you will need to modify:

1. `DrawRend::rasterize_triangle`
2. `TexTri::color`
3. `Texture::sample`
4. `Texture::get_level`

**Extra Credit:** Implement [anisotropic filtering](https://en.wikipedia.org/wiki/Anisotropic_filtering) or [summed area tables](https://en.wikipedia.org/wiki/Summed_area_table). Show comparisons of your method to nearest, bilinear, and trilinear sampling. Use `clock()` to measure the relative performance of the methods.


## Section III: (optional, possible extra credit) Art Competition

### Part 7: Draw something interesting!

Use your newfound powers to render something fun. You can look up the svg specifications online for matrix transforms and for Point, Line, Polyline, Rect, Polygon, and Group classes. The ColorTri and TexTri are our own inventions, so you can intuit their parameters by looking at the svgparser.cpp file. You can either try to draw something "by hand" or try to output an interesting pattern programmatically. For example, we wrote some simple programs to generate the texture mapped *svg* files in the *texmap* directory as well as the color wheel in *basic/test7.svg*.

Flex your right or left brain -- either show us your artistic side, or generate awesome procedural patterns with code. This could involve a lot of programming either inside or outside of the codebase! If you write a script to generate procedural *svg* files, include it in your submission and briefly explain how it works.

**Tips and guidelines for your submission:**

* Your resulting *png* screenshot should be 800x800 resolution. Keep this in mind when writing your *svg* file.
* Use the GUI's `'S'` functionality to save your screenshot as a *png*. Don't take your own screenshot of your rasterized result, or you'll ruin the quality of your hard work!
* Note: The rasterizer cannot display svg Path elements, so do not include any curves in the svg you wish to load.

Students will vote on their favorite submissions and the top voted submission(s) will receive extra credit! More details regarding the art competition will be announced next week. Stay in tune!

## Submission

Please consult this article on [how to build and submit assignments for CS 184](classarticle:7).

You will submit your code as well as some deliverables (see below) in a webpage project write-up.

### Project write-up guidelines and instructions

We have provided a simple HTML skeleton in *index.html* found within the *docs* directory to help you get started and structure your write-up.

You are also welcome to create your own webpage report from scratch using your own preferred frameworks or tools. However, **please follow the same overall structure as described in the deliverables section below**.

The goals of your write-up are for you to (a) think about and articulate what you've built and learned in your own words, (b) have a write-up of the project to take away from the class. Your write-up should include:

* An overview of the project, your approach to and implementation for each of the parts, and what problems you encountered and how you solved them. Strive for clarity and succinctness.
* On each part, make sure to include the results described in the corresponding Deliverables section in addition to your explanation. If you failed to generate any results correctly, provide a brief explanation of why.
* The final (optional) part for the art competition is where you have the opportunity to be creative and individual, so be sure to provide a good description of what you were going for and how you implemented it.
* Clearly indicate any extra credit items you completed, and provide a thorough explanation and illustration for each of them.

The write-up is one of our main methods of evaluating your work, so it is important to spend the time to do it correctly and thoroughly. Plan ahead to allocate time for the write-up well before the deadline.

### <a name="rubric"></a>Project Write-up Deliverables and Rubric

#### Overview
Give a high-level overview of what you implemented in this project. Think about what you've built as a whole. Share your thoughts on what interesting things you've learned from completing the project.

#### Part 1 (20 pts)
* Walk through how you rasterize triangles in your own words. 
* Explain how your algorithm is no worse than one that checks each sample within the bounding box of the triangle.
* Show a *png* screenshot of *basic/test4.svg* with the default viewing parameters and with the pixel inspector centered on an interesting part of the scene.
* *Extra credit:* Explain any special optimizations you did beyond simple bounding box triangle rasterization, with a timing comparison table (we suggest using the c++ `clock()` function around the `svg.draw()` command in `DrawRend::redraw()` to compare millisecond timings with your various optimizations off and on).

#### Part 2 (20 pts) 
* Walk through how you implemented supersampling. Why is supersampling useful? What modifications did you make to the rasterization pipeline in the process? Explain how you used supersampling to antialias your triangles.
* Show *png* screenshots of *basic/test4.svg* with the default viewing parameters and sample rates 1, 4, and 16 to compare them side-by-side. Position the pixel inspector over an area that showcases the effect dramatically; for example, a very skinny triangle corner. Explain why these results are observed.
* *Extra credit:* If you implemented alternative antialiasing methods, describe them and include comparison pictures demonstrating the difference between your method and grid-based supersampling.

#### Part 3 (10 pts)

* Create an updated version of *svg/transforms/robot.svg* with cubeman doing something more interesting, like waving or running. Feel free to change his colors or proportions to suit your creativity. Save your *svg* file as *my_robot.svg* in your *docs/* directory and show a *png* screenshot of your rendered drawing in your write-up. Explain what you were trying to do with cubeman in words.

#### Part 4 (10 pts)
* Explain barycentric coordinates in your own words and use an image to aid you in your explanation. One idea is to use a *svg* file that plots a single triangle with one red, one green, and one blue vertex, which should produce a smoothly blended color triangle.
* Show a *png* screenshot of *svg/basic/test7.svg* with default viewing parameters and sample rate 1. If you make any additional images with color gradients, include them.

#### Part 5 (15 pts)
* Explain pixel sampling in your own words and describe how you implemented it to perform texture mapping. Briefly discuss the two different pixel sampling methods, nearest and bilinear.
* Check out the *svg* files in the *svg/texmap/* directory. Use the pixel inspector to find a good example of where bilinear sampling clearly defeats nearest sampling. Show and compare four *png* screenshots using nearest sampling at 1 sample per pixel, nearest sampling at 16 samples per pixel, bilinear sampling at 1 sample per pixel, and bilinear sampling at 16 samples per pixel.
* Comment on the relative differences. Discuss when there will be a large difference between the two methods and why.

#### Part 6 (25 pts)
* Explain level sampling in your own words and describe how you implemented it for texture mapping.
* You can now adjust choosing between pixel sampling and level sampling as well as adjust the number of samples per pixel. Analyze the tradeoffs between speed, memory usage, and antialiasing power between the various techniques at different zoom levels.
* Show at least one example (using a *png* file you find yourself) comparing all four combinations of one of `L_ZERO` and `L_NEAREST` with one of `P_NEAREST` and `P_LINEAR` at a zoomed out viewpoint.
  * To use your own *png*, make a copy of one of the existing *svg* files in *svg/texmap/* (or create your own modelled after one of the provided *svg* files). Then, near the top of the file, change the texture filename to point to your own *png*. From there, you can run ./draw and pass in that svg file to render it and then save a screenshot of your results.
* *Extra credit:* If you implemented any extra filtering methods, describe them and show comparisons between your results with the other above methods.

#### (Optional) Part 7
* Save your best *svg* file as *competition.svg* in your *docs/* directory, and show us a 800x800 *png* screenshot of it in your write-up!
* Explain how you did it. If you wrote a script to generate procedural *svg* files, include it in your submission in the *src/* directory and briefly explain how it works.

### Website tips and advice

* Your main report page should be called *index.html*.
* Be sure to include and turn in all of the other files (such as images) that are linked in your report!
* Use only *relative* paths to files, such as `"./images/image.jpg"`
* Do *NOT* use absolute paths, such as `"/Users/student/Desktop/image.jpg"`
* Pay close attention to your filename extensions. Remember that on UNIX systems (such as the instructional machines), capitalization matters. `.png != .jpeg != .jpg != .JPG`
* Be sure to adjust the permissions on your files so that they are world readable. For more information on this please see this [tutorial](http://www.grymoire.com/Unix/Permissions.html").
* Start assembling your webpage early to make sure you have a handle on how to edit the HTML code to insert images and format sections.
