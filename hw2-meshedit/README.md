# Assignment 2

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_2.jpg)


In this assignment you will explore some of the geometric topics covered in lecture. You will build Bezier curves and surfaces using de Casteljau's algorithm, manipulate half-edge meshes, and implement loop subdivision! When you are finished, you will have created a tool that allows you to load and edit basic COLLADA mesh files that are now used by many major modeling packages and real time graphics engines. Lastly, you can optionally (for possible extra credit) design your own COLLADA file by building your own mesh using free software such as [Blender](http://blender.org).


## Logistics

### Deadline

Project 2 is due **Tuesday, February 26th at 11:59pm.** Your final submission must include both your final code and your final write-up. Assignments that are turned in after 11:59pm are a full day late -- there are no late minutes or late hours.


### Getting started

As before, accept the assignment in your CS184 website profile, follow the instructions on Github Classroom, and clone your generated repo (not the class skeleton).

    $ git clone <YOUR_PRIVATE_REPO>

Again, make sure Github Pages is enabled for your assignment, and please consult this article for more information on [how to build and submit assignments for CS 184](https://cs184.eecs.berkeley.edu/sp19/article/8/building-and-submitting-cs184-as). 

**We recommend that you accumulate deliverables into sections in your write-up as you work through the project.** This time, we've included the corresponding writeup instructions at the end of each part of the spec, as well as in the [write-up guidelines and deliverables section](#rubric) at the end. 


## Project structure

The project has 7 parts, divided into 3 sections, worth a total of 100 possible points. Some require only a few lines of code, while others are more substantial.

**Section I: Bezier Curves and Surfaces**

* Part 1: Bezier curves with 1D de Casteljau subdivision (10 pts)
* Part 2: Bezier surfaces with separable 1D de Casteljau subdivision (15 pts)

**Section II: Loop Subdivision of General Triangle Meshes**

* Part 3: Average normals for half-edge meshes (10 pts)
* Part 4: Half-edge flip (15 pts)
* Part 5: Half-edge split (15 pts)
* Part 6: Loop subdivision for mesh upsampling (25 pts)

**Section III: (optional, possible extra credit) Mesh Competition**

* Part 7: Design your own mesh!


## Using the GUI

You can run the executable on any single COLLADA file (.dae, .bzc, .bez):

```
./meshedit <PATH_TO_COLLADA_FILE>
```

For example, in Section I Part 1, you will be able to load Bezier curves by running a command such as:

```
./meshedit ../bzc/curve1.bzc
```

And in Section I Part 2, you will be able to load Bezier surfaces by running a command such as:

```
./meshedit ../bez/teapot.bez
```

Note that the GUI for Part 1 is somewhat different, and will be described further in the [Part 1 spec](#part1gui). When you run the application from Part 2 onwards, you will see a picture of a mesh made of triangles.

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_3.jpg)

As you move the cursor around the screen, you'll notice that mesh elements (faces, edges, and vertices) under the cursor get highlighted. Clicking on one of these elements will display some information about the element and its associated data.

Here is the full specification on keyboard controls for the GUI:

|Key | Action|
|:---------------:|------|
|<kbd>F</kbd>     | Flip the selected edge |
|<kbd>S</kbd>     | Split the selected edge|
|<kbd>U</kbd>     | Upsample the current mesh |
|<kbd>I</kbd>     | Toggle information overlay |
|<kbd>N</kbd>     | Select the next halfedge |
|<kbd>T</kbd>     | Select the twin halfedge |
|<kbd>W</kbd>     | Switch to GLSL shaders |
|<kbd>0-9</kbd>   | Switch between GLSL shaders |
|<kbd>Q</kbd>     | Toggle using area-averaged normals |
|<kbd>R</kbd>     | Recompile shaders |
|<kbd>SPACE</kbd> | Reset camera to default position |

There are also a few mouse commands:

* **Click and drag a vertex** to change its position
* **Click and drag the background** or **right click** to rotate the camera.
* **Scroll** to adjust the camera zoom.


Of these commands, you will implement the following, which will allow you to modify the mesh in a variety of ways.

1. Local flip (**F**) and split (**S**) operations, which modify the mesh in a small neighborhood around the currently selected mesh element. These will be implemented in Part 4 and Part 5.
2. Loop subdivision (**U**), which refines and smooths the entire mesh. This will be implemented in Part 6.

Note that each COLLADA file may contain multiple mesh objects. More generally, a COLLADA file describes a __scene graph__ (much like SVG) that is a hierarchical representation of all objects in the scene (meshes, cameras, lights, etc.), as well as their coordinate transformations. Global resampling methods will be run on whichever mesh is currently selected.


## Getting Acquainted with the Starter Code

Before you start, here is some basic information on the structure of the starter code. **Your code for all parts will be contained inside *student_code.cpp***.

For Bezier curves and surfaces (Section I), you'll be filling in member functions of the  `BezierCurve` and `BezierPatch` classes, declared in *bezierCurve.\** and *bezierPatch.\**.

For half-edge meshes (Section II), you'll be filling in member functions of the `HalfedgeMesh` class, declared in *halfEdgeMesh.\**.

**We have put dummy definitions for all the functions you need to modify inside *student_code.cpp*, where you'll be writing your code.**

## Section I: Bezier Curves and Surfaces

In Section I, we take a closer look at Bezier curves and surfaces. In computer graphics, Bezier curves and surfaces are parametric curves and surfaces that are frequently used to model smooth and indefinitely scalable curves and surfaces.

A Bezier curve of degree $n$ is defined by $(n + 1)$ control points, and is a parametric line based on a single parameter $t$, which ranges between 0 and 1.

Similarly, a Bezier surface of degree $(n, m)$ is defined by $(n + 1)(m + 1)$ control points, and is a parametric surface based on two parameters $u$ and $v$, both still ranging between 0 and 1.

Using de Casteljau's algorithm, we can evaluate these parametric curves and surfaces for any given set of parameters.

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_20.jpg)

### Part 1: Bezier curves with 1D de Casteljau subdivision (10 pts)

[**Relevant lecture: 7**](https://cs184.eecs.berkeley.edu/sp19/lecture/7/)

In Part 1, we will work with generic Bezier curves, though we will look at cubic Bezier curves as our motivating example and extend the concept from there to support any order Bezier curve.

To get started, take a look at *bezierCurve.h* and examine the protected variables defined within the class. Let's briefly go over the purpose of ones you will primarily be concerned with:

* `std::vector<Vector2D> controlPoints`: A vector containing the exact control points that define the Bezier curve. This vector is initialized with the control points of the Bezier curve file passed in.
* `std::vector< std::vector<Vector2D> > evaluatedLevels`: A 2D vector containing the evaluated points at each level of subdivision when applying de Casteljau's algorithm. Initially, it contains a single vector representing the starting level containing just the original control points. You should add additional levels with their respective "intermediate control points" every time `evaluateStep` is called. For example, a cubic Bezier curve will start with a single initial level containing the original control points. After three calls to `evaluateStep` it will have four total levels, where the final leve contains just a single evaluated point that represents $B(t)$ (the Bezier curve evaluated at $t$).

Implement `evaluateStep`, which looks at the control points from the most recently evaluated level and performs de Casteljau's algorithm to compute the next level of intermediate control points. **Each call to this function performs _one_ step of the algorithm.** These intermediate control points at each level should be appropriately stored into the member variable `evaluatedLevels`, which is used by the program to render the Bezier curve.

#### Implementation Notes
* `evaluatedLevels` is seeded with the original control points of the Bezier curve, so you should already have a "most recent level" to look at and use to compute your first set of intermediate control points.
* **DO NOT** use or modify the variable `eval_level` defined in `bezierCurve.h`. This is used internally for rendering the curve.
* `std::vector` is similar to Java's `ArrayList` class; you should use `std::vector`'s `push_back` method to add elements, which is analogous to Java's `ArrayList`'s `append` method.

Recall from lecture that de Casteljau's algorithm gives us the following recursive step that we can repeatedly apply to evaluate a Bezier curve:

Given $n$ points $p_0, ..., p_n$, compute a new set of $n-1$ points $p_0', ..., p_{n-1}'$ through linear interpolation (lerp):

$$p_i' = lerp(p_i, p_{i+1}, t)$$

where lerp($p_i$, $p_{i+1}$, $t$) = $(1 - t)p_i + tp_{i+1}$. 

`evaluateStep` should return immediately if the Bezier curve has already been completely evaluated at $t$ (i.e. `evaluateStep` has already been called enough times to completely solve for the point at $B(t)$).

Check your implementation by running the program with this syntax: `./meshedit <path to .bzc file>`:


    ./meshedit ../bzc/curve1.bzc
    

*bzc/curve1.bzc* is a cubic Bezier curve, whereas *bzc/curve2.bzc* is a degree-4 Bezier curve. Feel free to play around with higher-order Bezier curves by creating your own *bzc* files.
    

#### <a name="part1gui"></a>Using the GUI: Part 1

For this Part only, the GUI is a little different. There are two keyboard commands:

|Key | Action|
|:---------------:|------|
|<kbd>E</kbd>     | Perform one call to `evaluateStep`; will cycle through the levels once fully evaluated |
|<kbd>C</kbd>     | Toggles whether or not the entirely evaluated Bezier curve is drawn to the screen|

Step through the evaluation of $B(t)$ by repeatedly pressing **E** to verify your implementation is correct. Toggle the Bezier curve using **C** to check that the  curve is generated correctly based on its control points.

You can also use your mouse to:

* **Click and drag** the control points to move them and see how your Bezier curve (and all intermediate control points) changes accordingly
* **Scroll** to move the evaluated point along the Bezier curve and see how the intermediate control points move along with it. This is essentially varying $t$ between 0.0 and 1.0


#### For Your Writeup: Part 1
* Briefly explain de Casteljau's algorithm and how you implemented it in order to evaluate Bezier curves.
* Take a look at the provided *bzc* files and create your own Bezier curve with **six (6)** control points of your own choosing. Use this Bezier curve for your screenshots below.
* Show screenshots of each step of the evaluation from the original control points down to the final evaluated point. Use the keyboard command **E** to step through. Toggle **C** to show the completed curve as well.
* Move the points around and modify the value of $t$ by scrolling and show us a slightly different Bezier curve.

### Part 2: Bezier surfaces with separable 1D de Casteljau subdivision (15 pts)

[**Relevant lecture: 7**](https://cs184.eecs.berkeley.edu/sp19/lecture/7/)

In Part 2, we will work only with cubic Bezier surfaces.

To get started, take a look at *bezierPatch.h* and examine the class definition. In this part, you will be working with:

* `std::vector< std::vector<Vector3D> > controlPoints`: A 2D vector representing a 4x4 grid of control points that define the cubic Bezier surface. This variable is initialized with all 16 control points.
* `Vector3D evaluate(double u, double v) const`: You will fill this function in, which evaluates the Bezier curve at parameters $(u, v)$. In mathematical terms, it computes $B(u, v)$.
* `Vector3D evaluate1D(std::vector<Vector3D> points, double t) const`: An optional helper function that you might find useful to implement to help you with your implementation of `evaluate`. Given an array of 4 points that lie on a single curve, evaluates the curve at parameter $t$ using 1D de Casteljau subdivision.

In class, we cover three different methods for evaluating Bezier surfaces. (1) "Separable 1D de Casteljau", in which we do 1D de Casteljau in $uv$ and then in $v$, (2) "2D de Casteljau", in which we perform iterated bilinear interpolation in both $u$ and $v$, and (3) algebraic evaluation using the binomial formulation and Bernstein polynomials. In this project, we will use the first method, "separable 1D de Casteljau".

Implement `evaluate`, which completely evaluates the Bezier surface at parameters $u$ and $v$. Unlike Part 1, you will not perform just a single step at a time -- you will instead completely evaluate the Bezier surface using the specified parameters. This function should return that final evaluated point.

Use the following algorithm to repeatedly apply separable 1D de Casteljau's algorithm in both dimensions to evaluate the Bezier surface:

    For each row i:
      Let q(i, u) := apply de Casteljau's algorithm with parameter $u$ to the i-th row of control points

    Let p(u, v) := apply de Casteljau's algorithm with parameter $v$ to all q(i, u) 
    Return p(u, v)

If your implementation is correct, you should see a teapot by running the following command:

    ./meshedit ../bez/teapot.bez


#### For Your Writeup: Part 2
* Briefly explain how de Casteljau's algorithm extends to Bezier surfaces and how you implemented it in order to evaluate Bezier surfaces.
* Show a screenshot of a rendering of *bez/teapot.bez*. (Note the file extension!) 


## Section II: Loop Subdivision of General Triangle Meshes

In Section I, we dealt with Bezier curves and surfaces, parametric functions that were defined by a set of control points. Through de Casteljau's algorithm, we performed subdivision that allowed us to evaluate those functions. 

With Bezier curves, we performed 1D subdivision, and evaluation steps were relatively simple since adjacent control points were trivially retrieved from a 1D vector. With Bezier surfaces, we applied the same 1D subdivision concepts in both dimensions, and evaluation steps were a little more complicated, but adjacent control points were still easily retrieved from the 2D grid of control points.

What about generic triangle meshes? Here, the only rules are that our mesh is made up of triangles and that the triangles connect to each other through their edges and vertices. There is no constraining 2D grid as before with Bezier surfaces. As a result, adjacency is unfortunately no longer trivial. Enter the half-edge data structure, a powerful and popular data structure commonly used to store mesh entities and their connectivity information. We'll be working with this data structure for the rest of the project.


### Part 3: Average normals for half-edge meshes (10 pts)

[**Relevant lecture: 8**](https://cs184.eecs.berkeley.edu/sp19/lecture/8/)

**Important Note:** Before diving into Section II, be sure to first consult lecture 8 for a refresher on half-edges, and read [this article](https://cs184.eecs.berkeley.edu/sp19/article/15/the-half-edge-data-structure) to help you navigate the `HalfedgeMesh` class, which you will use extensively in the next section of the project. After reading it, you will have a better understanding of the details about the half-edge data structure and its implementation in the starter code.

For Part 3, make sure you understand the code given for a `printNeighborPositions` function as well.

In this part, you will implement the `Vertex::normal` function inside *student_code.cpp*. This function returns the area-weighted average normal vector at a vertex, which can then be used for more realistic local shading compared to the default flat shading technique.

In order to compute this value, you will want to use a `HalfedgeIter` to point to the `Halfedge` you are currently keeping track of. A `HalfedgeIter` (analogously `VertexIter`, `EdgeIter`, and `FaceIter`) is essentially a pointer to a `Halfedge` (respectively `Vertex`, `Edge`, and `Face`), in the sense that you will use `->` to dereference its member functions. Also, you can test whether two different iterators point to the same object using `==`, and you can assign one iterator to point to the same thing as another using `=` (this will NOT make the pointed-to objects have the same value, just as with pointers!). 

**Technical implementation caveat**: For this part only, you're implementing a `const` member function, which means you need to use `HalfedgeCIter`s instead of `HalfedgeIter`s. These merely promise not to change the values of the things they point to.

The relevant member functions for this task are `Vertex::halfedge()`, `Halfedge::next()` and `Halfedge::twin()`. You will also need the public member variable `Vector3D Vertex::position`.

How you might use these to begin implementing this function:

    Vector3D n(0,0,0); // initialize a vector to store your normal sum
    HalfedgeCIter h = halfedge(); // Since we're in a Vertex, this returns a halfedge
                                 // pointing _away_ from that vertex
    h = h->twin(); // Bump over to the halfedge pointing _toward_ the vertex.
                   // Now h->next() will be another edge on the same face,
                   // sharing the central vertex.
                   
At this point, you should

1. Save a copy of `h`'s value in another `HalfedgeCIter h_orig`.
2. Start a `while` loop that ends when `h == h_orig`.
3. Inside each loop iteration: 
    * Accumulate area-weighted normal of the current face in the variable `n`. You can do this by using the cross product of triangle edges. We've defined the cross product for you, so don't re-implement it yourself! Since the cross product of two vectors has a norm equal to twice the area of the triangle they define, these vectors are *already area weighted*!
    * Once you've added in the area-weighted normal, you should advance `h` to the halfedge for the next face by using the `next()` and `twin()` functions.
4. After the loop concludes, return the re-normalized unit vector `n.unit()`.


After completing this part, load up a *dae* such as *dae/teapot.dae* and press **W** to switch to GLSL shaders and then press **Q** to toggle area-averaged normal vectors (which will call on your `Vertex::normal` function). Here's an example of what *dae/teapot.dae* should look like with correctly implemented area-averaged normals.

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_18.jpg)

It should NOT look like this!

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_19.jpg)

#### For Your Writeup: Part 3
* Briefly explain what you did.
* Show mesh renderings of *dae/teapot.dae*, comparing the default OpenGL shading with and without smoothed normals (use **Q** to switch between face normals and average vertex normals). (Note the file extension!)


### Part 4: Half-edge flip (15 pts)

**Relevant lectures: [8](https://cs184.eecs.berkeley.edu/sp19/lecture/8/)**

Now you should be a little more comfortable traversing the half-edge pointers. In this task, you will implement a more substantial method: a local remeshing operation that "flips" an edge, implemented inside the method `HalfedgeMesh::flipEdge` in file *student_code.cpp*.  

More precisely, suppose we have a pair of triangles $(a,b,c)$ and $(c,b,d)$. After flipping the edge $(b,c)$, we should now have triangles $(a,d,c)$ and $(a,b,d)$:

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_9.jpg)

Your solution should:

 * Never flip boundary edges (just return immediately if either neighboring face is a boundary loop). Every object has a useful `boundary()` function that can tell you if it is or is not on the boundary.
 * Perform only a constant amount of work -- the cost of flipping a single edge should **not** be proportional to the size of the mesh!
 * Not add or delete any elements.  Since there are the same number of mesh elements before and after the flip, you should only need to reassign pointers.

The biggest challenge in properly implementing this operation (as well as split) is making sure that all the pointers still point to the right place in the modified mesh. An easy recipe for ensuring that all pointers are still valid after any general remeshing operation is:

 1. Draw a picture and/or write down a list of all the elements (vertices, edges, faces, halfedges) that will be needed from the original mesh.
 2. Draw a picture and/or write down a list of all the elements that should appear in the modified mesh.
 3. Allocate any new elements that are needed in the modified mesh, but do not appear in the original mesh (only relevant for the next part).
 4. For every element in the "modified" picture, set **all** of its pointers -- even if they didn't change. For instance, for each halfedge, make sure to set `next`, `twin`, `vertex`, `edge`, and `face` to the correct values in the new (modified) picture. For each vertex, edge, and face, make sure to set its `halfedge` pointer. A convenience method `Halfedge::setNeighbors()` has been created for the purpose of setting all pointers inside a halfedge at once.

The reason for setting all the pointers (and not just the ones that changed) is that it is very easy to miss a pointer, causing your code to fail. Once the code is **working**, you can remove these unnecessary assignments if you wish.

*Tip:* You can check which other objects in the mesh point to a given object by using the debug functions `check_for` inside `HalfEdgeMesh`. There's more information about these functions at the bottom of the halfedge documentation article.


#### For Your Writeup: Part 4
* Briefly explain how you implemented the half-edge flip operation and describe any interesting implementation/debugging tricks you used.
* Show a screenshot of a mesh before and after some edge flips.
* Write about your eventful debugging journey, if you experienced one.


### Part 5: Half-edge split (15 pts)

**Relevant lectures: [8](https://cs184.eecs.berkeley.edu/sp19/lecture/8/)**

This time, you will make a different local modification to the mesh in the neighborhood of an edge, called a __split__. In particular, suppose we have a pair of triangles $(a,b,c)$ and $(c,b,d)$. The edge $(b,c)$ is split by inserting a new vertex m at its midpoint and connecting it to the opposite vertices $a$ and $d$, yielding four triangles:

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_10.jpg)

This task is a bit tricker than "flip" because there are more pointers to keep track of, and you will have to allocate new mesh elements this time (e.g., two new triangles, three edges, some halfedges...).  Your implementation should:

 * Ignore requests to split boundary edges, unless you are trying for extra credit (just return immediately if either neighboring face is a boundary loop). Note that splitting a boundary edge **does** make sense, but flipping a boundary edge **does not** make sense.
 * Assign the position of the new vertex to the midpoint of the original edge, i.e., the average of its two endpoints (see `Vertex::position`).
 * Perform only a constant amount of work -- the cost of splitting a single edge should **not** be proportional to the size of the mesh!
 * Allocate only as many new elements as needed; there should be no "orphaned" elements that are not connected to the rest of the mesh.

To obtain a correct implementation, you might try following the same "recipe" given in the previous task (though clever, clean, and simple alternatives are of course always welcome). To verify that your implementation works correctly, try flipping some edges that you've split, and splitting some edges that you flipped. Further, alternate between flipping and splitting edges at least ten times in nearby and far-apart regions of the mesh and check that your mesh changes appropriately.

*Tip:* You can check which other objects in the mesh point to a given object by using the debug functions `check_for` inside `HalfEdgeMesh`. There's more information about these functions at the bottom of the halfedge documentation article.

**Extra Credit:** Support edge split for boundary edges. For this, you will need to carefully read the section about the "virtual boundary face" in the halfedge article. In this case, you will split the edge in half but only split the face that is non-boundary into two.


#### For Your Writeup: Part 5
* Briefly explain how you implemented the half-edge split operation and describe any interesting implementation/debugging tricks you used.
* Show a screenshot of a mesh before and after some edge splits.
* Show a screenshot of a mesh before and after a combination both edge splits and edge flips.
* Write about your epic debugging quest, if you went on one.
* If you implemented support for boundary edges, give some screenshot examples of your implementation properly handling split operations on bounary edges.


### Part 6: Loop subdivision for mesh upsampling (25 pts)

**Relevant lectures: [8](https://cs184.eecs.berkeley.edu/sp19/lecture/8/)**

Now, we can leverage the previous two parts to make implementing the mesh topology changes in Loop subdivision very simple! In this task, you will implement the whole Loop subdivision process inside the `MeshResampler::upsample` in *student_code.cpp*.

Loop subdivision is somewhat analogous to upsampling using some interpolation method in image processing: we may have a low-resolution polygon mesh that we wish to upsample for display, simulation, etc.  Simply splitting each polygon into smaller pieces doesn't help, because it does nothing to alleviate blocky silhouettes or chunky features. Instead, we need an upsampling scheme that nicely interpolates or approximates the original data. Polygon meshes are quite a bit trickier than images, however, since our sample points are generally at _irregular_ locations, i.e., they are no longer found at regular intervals on a grid.

Loop subdivision consists of two basic steps:

1. Change the mesh topology: split each triangle into four by connecting edge midpoints (sometimes called "4-1 subdivision").
2. Update vertex positions as a weighted average of neighboring positions.

4-1 subdivision does this to each triangle:

<center>
<img src="http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_11.jpg" width="500px" align="middle"/>
</center>

And the following picture depicts the correct weighting for the new averaged vertex positions:

<center>
<img src="http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_13.jpg" width="500px" align="middle"/>
</center>

Written out, the new position of an old vertex is 

    (1 - n*u) * original_position + u * neighbor_position_sum
    
where `n` is the number of neighboring vertices, `u` is a constant as depicted in the figure above, `original_position` is the vertex's original position, and `neighbor_position_sum` is the sum of all neighboring vertices' positions.
    
The position for a newly created vertex v that splits an edge AB connecting vertices A and B and is flanked by opposite vertices C and D across the two faces connected to AB in the original mesh will be 

    3/8 * (A + B) + 1/8 * (C + D)

If we repeatedly apply these two steps, we will converge to a smoothed approximation of our original mesh.  In this task you will implement Loop subdivision, leveraging the split and flip operations to handle the topology changes.  In particular, you can achieve a 4-1 subdivision by applying the following strategy:

1. Split every edge of the mesh in any order whatsoever.
2. Flip any new edge that touches a new vertex and an old vertex. *Note*: Every original edge will now be represented by 2 edges, you *should not* flip these edges, because they are always already along the boundary of the 4 way divided triangles. In the diagrams below, you should only flip the blue edges that connect an old and new vertex, but you should not flip any of the black new edges.

The following pictures (courtesy Denis Zorin) illustrate this idea:

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_14.jpg)

#### Implementation walkthrough

For Loop subdivision, we have also provided some additional data members that will make it easy to keep track of the data you need to update the connectivity and vertex positions. In particular:

   * `Vertex::newPosition` can be used as temporary storage for the new position (computed via the weighted average above).  Note that you should _not_ change the value of `Vertex::position` until _all_ the new vertex positions have been computed -- otherwise, you are taking averages of values that have already been averaged!
   * Likewise, `Edge::newPosition` can be used to store the position of the vertices that will ultimately be inserted at edge midpoints.  Again, these values should be computed from the original values (before subdivision), and applied to the new vertices only at the very end. The `Edge::newPosition`value will be used for the position of the vertex that will appear along the old edge after the edge is split. We precompute the position of the new vertex before splitting the edges and allocating the new vertices because it is easier to traverse the simpler original mesh to find the positions for the weighted average that determines the positions of the new vertices.
   * `Vertex::isNew` can be used to flag whether a vertex was part of the original mesh, or is a vertex newly inserted by subdivision (at an edge midpoint).
   * `Edge::isNew` likewise flags whether an edge is a piece of an edge in the original mesh, or is an entirely new edge created during the subdivision step.

Given this setup, we strongly suggest that it will be easiest to implement subdivision according to the following "recipe" (though you are of course welcome to try doing things a different way!). The basic strategy is to _first_ compute the new vertex positions (storing the results in the `newPosition` members of both vertices and edges), and only _then_ update the connectivity. Doing it this way will be much easier, since traversal of the original (coarse) connectivity is much simpler than traversing the new (fine) connectivity. In more detail:

1. Mark all vertices as belonging to the original mesh by setting `Vertex::isNew` to `false` for all vertices in the mesh.
2. Compute updated positions for all vertices in the original mesh using the vertex subdivision rule, and store them in `Vertex::newPosition`.
3. Compute new positions associated with the vertices that will be inserted at edge midpoints, and store them in `Edge::newPosition`.
4. Split every edge in the mesh, being careful about how the loop is written.  In particular, you should make sure to iterate only over edges of the original mesh.  Otherwise, you will keep splitting edges that you just created!
5. Flip any new edge that connects an old and new vertex.
6. Finally, copy the new vertex positions (`Vertex::newPosition`) into the usual vertex positions (`Vertex::position`).

If you made the requested modification to the return value of `HalfedgeMesh::splitEdge()` (see above), then an edge split will now return an iterator to the newly inserted vertex, and the halfedge of this vertex will point along the edge of the original mesh. This iterator is useful because it can be used to (i) flag the vertex returned by the split operation as a new vertex, and (ii) flag each outgoing edge as either being new or part of the original mesh.  (In other words, Step 3 is a great time to set the members `isNew` for vertices and edges created by the split. It is also a good time to copy the `newPosition` field from the edge being split into the `newPosition` field of the newly inserted vertex.)

You might try implementing this algorithm in stages, e.g., _first_ see if you can correctly update the connectivity, _then_ worry about getting the vertex positions right. Some examples below illustrate the correct behavior of the algorithm.

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_12.jpg)


**Extra Credit:** *Support surfaces with boundary.* To do this, you will first need to make sure your edge split operation appropriately handles boundary edges (you do not need to change your edge flip function). You will also need to use a different weighted average for boundary vertices; see [Boier-Martin et al, "A Survey of Subdivision-Based Tools for Surface Modeling"](http://mrl.nyu.edu/~dzorin/papers/boiermartin2005sbt.pdf) for more information.

**Extra Credit:** *Implement additional subdivision schemes.* There are many alternatives to Loop subdivision.  Triangle subdivision schemes include Butterfly and [modified Butterfly](http://mrl.nyu.edu/~dzorin/papers/zorin1996ism.pdf) (which are interpolating rather than approximating) and [Sqrt(3)](https://www.graphics.rwth-aachen.de/media/papers/sqrt31.pdf) (which refines the mesh at a "slower" rate); the most popular subdivision scheme for quadrilateral meshes is Catmull-Clark. There are also special subdivision schemes for handling meshes with high degree vertices, e.g., extraordinary vertices, called [polar subdivision](http://www.cise.ufl.edu/research/SurfLab/papers/09bi3c2polar.pdf).


#### For Your Writeup: Part 6
* Briefly explain how you implemented the Loop subdivision and describe any interesting implementation/debugging tricks you used.
* Take some notes as well as some screenshots to record your observations of how meshes behave after Loop subdivision. What happens to sharp corners and edges? Can you lessen this effect by pre-splitting some edges?
* Load up *dae/cube.dae*. Try several iterations of Loop subdivision on the cube. Notice that the cube.dae becomes slightly asymmetric after repeated subdivision steps. Play around with this using flip and split. Can you pre-process the cube with flip and split so it subdivides symmetrically? Document these effects and explain why they occur, and how your pre-processing helps alleviate the effects.
* If you implemented any extra credit extensions, explain what you did and document how they work with screenshots.


## Section III: (optional, possible extra credit) Mesh Competition

### Part 7: Design your own mesh!

For this part, you will design your own COLLADA *.dae* file format mesh using the free program [Blender](http://blender.org). Our suggestion for a baseline starting point is to design a humanoid mesh with a head, two arms, and two legs. We've created a [Blender video tutorial](http://cs184.eecs.berkeley.edu/cs184_sp16_content/article_images/blender.mp4) and a [Blender tutorial article](https://cs184.eecs.berkeley.edu/sp19/article/16/blender-tutorial) to guide you through the basics of making a simple humanoid mesh.

Once you make your mesh, you should load it into *meshedit* and display what you've implemented! Subdivide it to smooth it out.

Here are some examples of a mesh-man before and after subdivision and also with an environment map reflection shader applied:

![](http://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/9_15.jpg)

Here are some ideas on things you might do in this part to express your creativity on your mesh:

* Add additional detail to your mesh - fingers, facial features.
* Google "box modeling" (images and videos) to get inspired. 
* Investigate additional functionality in Blender to design an alternative shape.
* Programmatically generate a mesh that approximates a 3D fractal or other complex shape.
* Write a super cool custom shader that makes your mesh look awesome.
* Implement additional geometric operations and demonstrate them on your mesh.

#### For Your Writeup: Part 7
* Save your best *dae* file as *competition.dae* in your *docs/* directory, and show us a screenshot of it in your write-up.
* Include a series of screenshots showing your original mesh, as well as your mesh after one and two rounds of subdivision. If you used any custom shaders, also include examples your mesh with those shaders applied.
* Describe what you did to enhance your mesh-man beyond the simple mesh-man described in the tutorial.

## Friendly advice from your GSIs

* Start early. Once again, this is a multi-part assignment, and you should manage your time appropriately. Remember that pernicious bugs can sap your time like no other, and always keep in mind [Hofstadter's Law](https://en.wikipedia.org/wiki/Hofstadter's_law).
* Parts 1 and 2 should be relatively straightforward to implement once you understand the concepts of de Casteljau's algorithm as well as Bezier curves and surfaces.
* Parts 3 through 6 will be trickier to write correctly, since they involve pointers (exposed to you as iterators). Make sure you test these parts (specifically parts 4 and 5) *together* on a few meshes -- alternately flip and split edges at least ten times in nearby and far apart regions of the mesh as a first test. Obviously, correct behavior does not imply correct code, but *in*correct behavior does imply incorrect code :)
* The optional Part 7 mesh competition for possible extra credit involves learning how to use a 3D modeling tool. The video tutorial will help, but it will probably be a new experience with a new tool for most people. We recommend getting started early in case you have questions. A good first milestone would be creating a simple box mesh in Blender and loading it into the *meshedit* application as a *.dae* file. This doesn't depend on any other part of the assignment, so you can do it anytime (perhaps as a nice break from chasing pointers...).
* Make sure you allocate enough time to do a good job on the write-up!


## Submission

Please consult this article on [how to build and submit assignments for CS 184](https://cs184.eecs.berkeley.edu/sp19/article/8/building-and-submitting-cs184-as).

You will submit your code as well as some deliverables (see below) in a webpage project write-up.

### Project write-up guidelines and instructions

We have provided a simple HTML skeleton in *index.html* found within the *docs* directory to help you get started and structure your write-up.

You are also welcome to create your own webpage report from scratch using your own preferred frameworks or tools. However, **please follow the same overall structure as described in the deliverables section below**.

The goals of your write-up are for you to (a) think about and articulate what you've built and learned in your own words, (b) have a write-up of the project to take away from the class. Your write-up should include:

* An overview of the project, your approach to and implementation for each of the parts, and what problems you encountered and how you solved them. Strive for clarity and succinctness.
* On each part, make sure to include the results described in the corresponding Deliverables section in addition to your explanation. If you failed to generate any results correctly, provide a brief explanation of why.
* The final (optional) part for the mesh competition is where you have the opportunity to be creative and individual, so be sure to provide a good description of what you were going for, what you did, and how you did it.
* Clearly indicate any extra credit items you completed, and provide a thorough explanation and illustration for each of them.

The write-up is one of our main methods of evaluating your work, so it is important to spend the time to do it correctly and thoroughly. Plan ahead to allocate time for the write-up well before the deadline.


### <a name="rubric"></a>Project Write-up Deliverables and Rubric

This rubric lists the basic, minimum requirements for your writeup. The content and quality of your write-up are extremely important, and you should make sure to at least address all the points listed below. The extra credit portions are intended for students who want to challenge themselves and explore methods beyond the fundamentals, and are not worth a large amount of points. In other words, don't necessarily expect to use the extra credit points on these projects to make up for lost points elsewhere.

#### Overview
Give a high-level overview of what you implemented in this project. Think about what you've built as a whole. Share your thoughts on what interesting things you've learned from completing the project.

#### Part 1

* Briefly explain de Casteljau's algorithm and how you implemented it in order to evaluate Bezier curves.
* Take a look at the provided *bzc* files and create your own Bezier curve with **six (6)** control points of your own choosing. Use this Bezier curve for your screenshots below.
* Show screenshots of each step of the evaluation from the original control points down to the final evaluated point. Use the keyboard command **E** to step through. Toggle **C** to show the completed curve as well.
* Move the points around and modify the value of $t$ by scrolling and show us a slightly different Bezier curve.

#### Part 2

* Briefly explain how de Casteljau's algorithm extends to Bezier surfaces and how you implemented it in order to evaluate Bezier surfaces.
* Show a screenshot of a rendering of *bez/teapot.bez*.

#### Part 3

* Briefly explain what you did.
* Show mesh renderings of *dae/teapot.dae*, comparing the default OpenGL shading with and without smoothed normals (use **Q** to switch between face normals and average vertex normals).

#### Part 4

* Briefly explain how you implemented the half-edge flip operation and describe any interesting implementation/debugging tricks you used.
* Show a screenshot of a mesh before and after some edge flips.
* Write about your eventful debugging journey, if you experienced one.

#### Part 5

* Briefly explain how you implemented the half-edge split operation and describe any interesting implementation/debugging tricks you used.
* Show a screenshot of a mesh before and after some edge splits.
* Show a screenshot of a mesh before and after a combination both edge splits and edge flips.
* Write about your epic debugging quest, if you went on one.
* If you implemented support for boundary edges, give some screenshot examples of your implementation properly handling split operations on bounary edges.

#### Part 6

* Briefly explain how you implemented the Loop subdivision and describe any interesting implementation/debugging tricks you used.
* Take some notes as well as some screenshots to record your observations of how meshes behave after Loop subdivision. What happens to sharp corners and edges? Can you lessen this effect by pre-splitting some edges?
* Load up *dae/cube.dae*. Try several iterations of Loop subdivision on the cube. Notice that the cube.dae becomes slightly asymmetric after repeated subdivision steps. Play around with this using flip and split. Can you pre-process the cube with flip and split so it subdivides symmetrically? Document these effects and explain why they occur, and how your pre-processing helps alleviate the effects.
* If you implemented any extra credit extensions, explain what you did and document how they work with screenshots.

#### Part 7

* Save your best *dae* file as *competition.dae* in your *docs/* directory, and show us a screenshot of it in your write-up.
* Include a series of screenshots showing your original mesh, as well as your mesh after one and two rounds of subdivision. If you used any custom shaders, also include examples your mesh with those shaders applied.
* Describe what you did to enhance your mesh-man beyond the simple mesh-man described in the tutorial.


### Website tips and advice

* Your main report page should be called *index.html*.
* Be sure to include and turn in all of the other files (such as images) that are linked in your report!
* Use only *relative* paths to files, such as `"./images/image.jpg"`
* Do *NOT* use absolulte paths, such as `"/Users/student/Desktop/image.jpg"`
* Pay close attention to your filename extensions. Remember that on UNIX systems (such as the instructional machines), capitalization matters. `.png != .jpeg != .jpg != .JPG`
* Be sure to adjust the permissions on your files so that they are world readable. For more information on this please see this [tutorial](http://www.grymoire.com/Unix/Permissions.html">http://www.grymoire.com/Unix/Permissions.html).
* Start assembling your webpage early to make sure you have a handle on how to edit the HTML code to insert images and format sections.
