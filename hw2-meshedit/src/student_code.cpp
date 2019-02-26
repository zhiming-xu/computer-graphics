#include "student_code.h"
#include "mutablePriorityQueue.h"

using namespace std;

namespace CGL
{
  template <class T>
  inline T lerp(const T &u, const T &v, double t)
  {
      return (1 - t) * u + t * v;
  }

  void BezierCurve::evaluateStep()
  {
    // TODO Part 1.
    // Perform one step of the Bezier curve's evaluation at t using de Casteljau's algorithm for subdivision.
    // Store all of the intermediate control points into the 2D vector evaluatedLevels.
    const vector<Vector2D>&last_level = evaluatedLevels[evaluatedLevels.size()-1];
    vector<Vector2D>new_level;
    for(int i=0;i<last_level.size()-1;++i)
    {
        Vector2D new_point = lerp<Vector2D>(last_level[i], last_level[i+1], this->t);
        new_level.push_back(new_point);
    }
    evaluatedLevels.push_back(new_level);
  }


  Vector3D BezierPatch::evaluate(double u, double v) const
  {
    // TODO Part 2.
    // Evaluate the Bezier surface at parameters (u, v) through 2D de Casteljau subdivision.
    // (i.e. Unlike Part 1 where we performed one subdivision level per call to evaluateStep, this function
    // should apply de Casteljau's algorithm until it computes the final, evaluated point on the surface)
    vector<Vector3D>bzc;
    for(int i=0;i<this->controlPoints.size();++i)
    {
        bzc.push_back(this->evaluate1D(this->controlPoints[i], u));
    }
    return this->evaluate1D(bzc, v);
  }

  Vector3D BezierPatch::evaluate1D(std::vector<Vector3D> points, double t) const
  {
    // TODO Part 2.
    // Optional helper function that you might find useful to implement as an abstraction when implementing BezierPatch::evaluate.
    // Given an array of 4 points that lie on a single curve, evaluates the Bezier curve at parameter t using 1D de Casteljau subdivision.
    if(points.size() == 1)
        return points[0];
    else
    {
        vector<Vector3D>new_level;
        for(int i=0;i<points.size()-1;++i)
        {
            new_level.push_back(lerp<Vector3D>(points[i], points[i+1], t));
        }
        return this->evaluate1D(new_level, t);
    }
 }



  Vector3D Vertex::normal( void ) const
  {
    // TODO Part 3.
    // TODO Returns an approximate unit normal at this vertex, computed by
    // TODO taking the area-weighted average of the normals of neighboring
    // TODO triangles, then normalizing.
    Vector3D ret = Vector3D(0, 0, 0);
    auto h = halfedge();
    h = h->twin();
    auto h_orig = h;
    do{
        //area += tmp_area;
        ret += h->face()->normal();
        //h = h->twin()->next();
        h = h->next()->twin();
    }while(h != h_orig);
    return ret.unit();
  }

  EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    // TODO Part 4.
    // TODO This method should flip the given edge and return an iterator to the flipped edge.
    auto h0 = e0->halfedge(), h3 = h0->twin();
    auto h1 = h0->next(), h2 = h1->next();
    auto h4 = h3->next(), h5 = h4->next();
    auto h6 = h1->twin(), h7 = h2->twin();
    auto h8 = h4->twin(), h9 = h5->twin();
    auto v1 = h3->vertex(), v0 = h0->vertex(), v2 = h2->vertex(), v3 = h8->vertex();
    auto f0 = h0->face(), f1 = h3->face();
    auto e1 = h6->edge(), e2 = h7->edge(), e3 = h8->edge(), e4 = h9->edge();
    // e0->halfedge's origin point, and next halfedges
    if(h0->isBoundary() || h1->isBoundary() || h2->isBoundary()||
       h3->isBoundary() || h4->isBoundary() || h5->isBoundary())
        return e0;
    // cout<<(h->face()==h_next->face()&&h_next->face()==h_next_next->face())<<endl;
    // cout<<(h_twin->face()==ht_next->face()&&ht_next->face()==ht_next_next->face())<<endl;
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

    return e0;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    // TODO Part 5.
    // TODO This method should split the given edge and return an iterator to the newly inserted vertex.
    // TODO The halfedge of this vertex should point along the edge that was split, rather than the new edges.
    // original halfedges
    if(e0->isBoundary())
        return newVertex();
    auto h0 = e0->halfedge(), h1=h0->next(), h2 = h1->next(),
         h3 = h0->twin(), h4 = h3->next(), h5 = h4->next();

    auto f0 = h0->face(), f1 = h3->face();
    // original vertices
    auto v0 = h0->vertex(), v1 = h3->vertex(),
         v2 = h2->vertex(), v3 = h5->vertex();
    auto new_vertex = newVertex();
    // add new edges
    auto e1 = h1->edge(), e2 = h2->edge(),
         e3 = h4->edge(), e4 = h5->edge();
    // add new halfedges
    auto h6 = newHalfedge(), h7 = newHalfedge(), h8 = newHalfedge(),
         h9 = newHalfedge(), h10 = newHalfedge(), h11 = newHalfedge();
    // add new edges
    auto e5 = newEdge(), e6 = newEdge(), e7 = newEdge();
    // add new faces
    auto f2 = newFace(), f3 = newFace();
    // new_vetex's location and halfedge it belongs to
    new_vertex->position = (v0->position + v1->position) / 2;
    new_vertex->halfedge() = h0;
    // set new halfedges
    h0->setNeighbors(h1, h3, new_vertex, e0, f0);
    h1->setNeighbors(h8, h1->twin(), v1, e1, f0);
    h2->setNeighbors(h6, h2->twin(), v2, e2, f2);
    h3->setNeighbors(h11, h0, v1, e0, f1);
    h4->setNeighbors(h10, h4->twin(), v0, e3, f3);
    h5->setNeighbors(h3, h5->twin(), v3, e4, f1);
    h6->setNeighbors(h7, h9, v0, e6, f2);
    h7->setNeighbors(h2, h8, new_vertex, e5, f2);
    h8->setNeighbors(h0, h7, v2, e5, f2);
    h9->setNeighbors(h4, h6, new_vertex, e6, f3);
    h10->setNeighbors(h9, h11, v3, e7, f3);
    h11->setNeighbors(h5, h10, new_vertex, e7, f1);
    // vertex assignments
    new_vertex->isNew = true;
    // edge assignments
    e5->halfedge() = h8;
    e6->halfedge() = h6;
    e7->halfedge() = h10;
    e0->isNew = false;
    e5->isNew = true;
    e6->isNew = false;
    e7->isNew = true;
    // face assignments
    f0->halfedge() = h0;
    f1->halfedge() = h3;
    f2->halfedge() = h2;
    f3->halfedge() = h4;
    // delete previous edge and face
    return e0->halfedge()->vertex();
  }



  void MeshResampler::upsample( HalfedgeMesh& mesh )
  {
    // TODO Part 6.
    // This routine should increase the number of triangles in the mesh using Loop subdivision.
    // Each vertex and edge of the original surface can be associated with a vertex in the new (subdivided) surface.
    // Therefore, our strategy for computing the subdivided vertex locations is to *first* compute the new positions
    // using the connectity of the original (coarse) mesh; navigating this mesh will be much easier than navigating
    // the new subdivided (fine) mesh, which has more elements to traverse. We will then assign vertex positions in
    // the new mesh based on the values we computed for the original mesh.


    // TODO Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
    // TODO and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
    // TODO a vertex of the original mesh.


    // TODO Next, compute the updated vertex positions associated with edges, and store it in Edge::newPosition.


    // TODO Next, we're going to split every edge in the mesh, in any order.  For future
    // TODO reference, we're also going to store some information about which subdivided
    // TODO edges come from splitting an edge in the original mesh, and which edges are new,
    // TODO by setting the flat Edge::isNew.  Note that in this loop, we only want to iterate
    // TODO over edges of the original mesh---otherwise, we'll end up splitting edges that we
    // TODO just split (and the loop will never end!)


    // TODO Now flip any new edge that connects an old and new vertex.


    // TODO Finally, copy the new vertex positions into final Vertex::position.

    HalfedgeIter h;
    VertexIter a, b, c, d;
    EdgeIter e1, e2, e3;
    FaceIter f1, f2, f3;
    int edgeNum = 0;

    for (auto e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++)
    {
      edgeNum += 1;
      e->isNew = false;
      h = e->halfedge();
      a = h->vertex();
      c = h->twin()->vertex();
      b = h->next()->twin()->vertex();
      d = h->twin()->next()->twin()->vertex();
      // new vertex postion
      e->newPosition = (((a->position + c->position) * 3 + (b->position + d->position)) / 8);
    }

    for (auto v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++)
    {
      v->isNew = false;
      Vector3D sum(0, 0, 0);

      int n = 0;
      h = v->halfedge();
      do{
        n += 1;
        sum += h->twin()->vertex()->position;
        h = h->twin()->next();
      } while (h != v->halfedge());

      double u;
      u = n==3?3.0 / 16:3.0 / (8 * n);
      v->newPosition = ((1 - n * u) * v->position + u * sum);
    }

    auto e = mesh.edgesBegin();
    for (int i = 0; i < edgeNum; i++, ++e)
    {
      auto v = mesh.splitEdge(e);
      v->newPosition = e->newPosition;
    }

    for (auto e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++)
    {
      if (e->isNew)
      {
        h = e->halfedge();
        if ((h->vertex()->isNew && !h->twin()->vertex()->isNew)
            || (!h->vertex()->isNew && h->twin()->vertex()->isNew))
        {
          mesh.flipEdge(e);
        }
      }
    }

    for (auto v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++)
    {
      v->position = v->newPosition;
    }

  }
}
