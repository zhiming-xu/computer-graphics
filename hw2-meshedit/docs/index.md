# CS 184: Computer Graphics and Imaging, Spring 2019

## Project 2: Mesh Editor

### Zhiming Xu

## Overview

In this project, we deal with meshes. First, we implement the necessary methods to draw a curve and surface with control points using de Casteljau's algorithm. Then we play with the elements of a mesh. We can edit the mesh represented by `halfedge` structure by flipping or splitting an edge. Moreover, with those two methods, we can upsample a mesh by loop subdivision, i.e., filling the mesh with more triangles to make it smoother.

## Section 1

### Part 1: Bezier curves with 1D de Casteljau subdivision

- I think 1D de Casteljau's algorithm is a recursive process of linear interpolation. It interpolates a set of vertices, reduces the total number of them by one, until only one left. In this process, the newly interpolated vertices serve as guideline to the Bezier curve we want to draw. That's is, the curve follows closely to them within the convex hull they define, and cross the last interpolated vertex. In function `evaluateStep` , I implement one step of interpolation, the pseudocode is shown below:

  ```c++
  evaluate1D()
  last_level = evaluatLevels[last]	// extract last level's vertices
  if last_level.size == 1
      return							// one vertex left, do nothing
  else
      initialize new_level			// more then one vertex, do lerp in order
      for each pair of adjacent vertices in last_levels
      	new_vertex = linear_interpolation(pair, t)	// t is the ratio
          add new_vertex to new_level	// add to next vertex level
  ```

- As shown in the picture below ![Berzier curve](./part1-1.png)

- By toggling `E`, the newly evaluated points become more and more

  ![First](./part1-3.png)

  ![First](./part1-4.png)

  ![First](./part1-5.png)

- By scrolling and moving control points, a different curve is shown below ![Different curve](./part1-2.png)

### Part 2: Bezier surfaces with separable 1D de Casteljau subdivision

- In 2D situation, we have a $4\times4$  array which stores $16$ control points and two ratios $u$ and $v$. Each row of the array stores the information to draw a Bezier curve in 1D. Thus we need to iterate on the first dimension to derive one control points from every row, then perform 1D de Cateljau's algorithm on them to obtain the final control point. Pseudocode listed below.

  ```c++
  // suppose the original control points stored in V
  evaluate(u, v)	// u and v are ratios, for row and column, respectively
  initialize an array new_control_points
  for i=0 to 3
      new_control_points.push_back(evaluate1D(V[i], u))
  evaluate1D(new_control_points, v)
  // helper function, do 1D lerp on an array of vertices recursively
  evaluate1D(points, t)
  if points.size == 1
      return points[0]
  else
  	calculate linear interpolation of points, store to new_level
  	return evaluate1D(new_level, t)
  ```

- As shown in the picture below ![Teapot](./2-1.png)

## Section 2: Loop Subdivision of General Triangle Meshes

### Part 3: Average normals for half-edge meshes

- I add the normals of cross products of all adjacent pairs of edges connected to the vertex, hence return the unit of this sum. This done by the following procedure.

  ```c++
  Vertex::normal()
  initialize return value ret
  for all faces connected to v
  	add face->normal to ret
  return ret
  ```

- The comparison is shown below.

  - Before: average vertex normals![Before](./3-2.png)
  - after: face normals![After](./3-1.png)

### Part 4: Half-edge flip

- I implement half-edge flip according in this [pattern](http://15462.courses.cs.cmu.edu/fall2015content/misc/HalfedgeEdgeOpImplementationGuide.pdf), the core idea is shown below.

  ![Flip](./4-1.png)

  The flipping process mainly consists of appropriate iterator assignments with no deletion. As shown in the figure above, the half-edges of the two triangles related to `e0`'s two half-edges' faces, namely, `f1`  and `f2`  have all rotated 90 degree counterclockwise. Consequently, we need to set the half-edges, vertices, edges and faces' iterators accordingly. The assignments are shown below. All variables' names are the same as in the figure.

  ```c++
  	h0->setNeighbors(h1, h3, v3, e0, f0);
      h1->setNeighbors(h2, h7, v2, e2, f0);
      h2->setNeighbors(h0, h8, v0, e3, f0);
      h3->setNeighbors(h4, h0, v2, e0, f1);
      h4->setNeighbors(h5, h9, v3, e4, f1);
      h5->setNeighbors(h3, h6, v1, e1, f1);
      h6->setNeighbors(h6->next(), h5, v2, e1, h6->face());
      h7->setNeighbors(h7->next(), h1, v0, e2, h7->face());
      h8->setNeighbors(h8->next(), h2, v3, e3, h8->face());
      h9->setNeighbors(h9->next(), h4, v1, e4, h9->face());
      // assign vertices' edge
      v0->halfedge() = h2;
      v1->halfedge() = h5;
      v2->halfedge() = h3;
      v3->halfedge() = h0;
      // assign e0's primary halfedge
      e0->halfedge() = h0;
      e1->halfedge() = h6;
      e2->halfedge() = h7;
      e3->halfedge() = h8;
      e4->halfedge() = h9;
      // face
      f0->halfedge() = h0;
      f1->halfedge() = h3;
  ```

  

- Before flipping![Before](./4-2.png)

  After flipping![After](./4-3.png)

- I have encountered a bug that when I flipped an edge twice, the mesh should have been the same as original, but one triangle adjacent to the edge was missing and there was a dark triangle instead. After checking the edge's adjacent parts', I found it was because I forgot to set the faces' half-edge to new value, thus they were missing from the mesh.

### Part 5: Half-edge split

- The mesh before and after the splitting operation is shown below. The annotated half-edges, vertices, faces and edges are added or moved during splitting.![Splitting](./5-3.png)

  Because I figured this out clearly before implementing this method, I didn't encounter too much trouble.

- Split only![S](./5-1.png)

  Split + flip![Sf](./5-2.png)

### Part 6: Loop subdivision for mesh upsampling

- I implemented loop subdivision in the following way

  - Iterate on mesh's edges, calculate their new position by the given formula `3/8 * (A + B) + 1/8 * (C + D)`.
  - Iterate on mesh's vertices, loop over its adjacent edges, find out how many neighbors it has. Then calculate its new position by `(1 - n*u) * original_position + u * neighbor_position_sum` where `u=3/16` if number of adjacent vertices, `n` is 3, otherwise, `u=3/(8*n).`
  - Next, split all edges in mesh, the new point's position is given by the new position calculated above.
  - Iterate on mesh's edges, flip those that connect a pair of new vertex and old vertex.
  - Iterate on mesh's vertices, set their position to new position calculated above.

- The sharp parts remain sharp with loop subdivision, but if I split the edges, they become smoother in next subdivision, as shown in the figures below (with and without splitting). The lower right and left areas of the first figure is sharper than those of the second figure.

  ![With](./5-4.png)

  ![Without](./5-5.png)

- I think split the diagonal (Shown in the first figure) helps to make the shape more symmetric. Besides, we can also continue to split edges evenly to enhance the effect. I think the reason why splitting evenly helps is because when doing loop subdivision, this will keep each vertex ordinary and prevent extraordinary points. Hence maintain the symmetry of the original mesh.

  ![Split diagonal](./6-1.png)

  ![Result](./6-2.png)

