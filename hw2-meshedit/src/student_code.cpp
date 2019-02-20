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
    auto h = e0->halfedge(), h_twin = h->twin();
    auto h_next = h->next(), h_next_next = h_next->next();
    auto ht_next = h_twin->next(), ht_next_next = ht_next->next();
    auto v1 = h_twin->vertex(), v3 = h->vertex();
    // e0->halfedge's origin point, and next halfedges
    if(h->isBoundary() || h_twin->isBoundary() || ht_next->isBoundary()||
       ht_next_next->isBoundary() || h_next->isBoundary() || h_next_next->isBoundary())
        return e0;
    // cout<<(h->face()==h_next->face()&&h_next->face()==h_next_next->face())<<endl;
    // cout<<(h_twin->face()==ht_next->face()&&ht_next->face()==ht_next_next->face())<<endl;
    h->vertex() = ht_next_next->vertex();
    h->next() = h_next_next;
    h_next_next->next() = ht_next;
    ht_next->next() = h;
    ht_next->face() = h->face();
    // e0->halfedge->twin's origin point, and next halfedges
    h_twin->vertex() = h_next_next->vertex();
    h_twin->next() = ht_next_next;
    ht_next_next->next() = h_next;
    h_next->next() = h_twin;
    h_next->face() = h_twin->face();
    // assign vertices' edge
    v1->halfedge() = h_next;
    v3->halfedge() = ht_next;
    // assign e0's primary halfedge
    //e0->halfedge() = h_twin;
    return e0;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    // TODO Part 5.
    // TODO This method should split the given edge and return an iterator to the newly inserted vertex.
    // TODO The halfedge of this vertex should point along the edge that was split, rather than the new edges.
    return newVertex();
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

    return;
  }
}
