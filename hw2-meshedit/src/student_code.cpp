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
    auto h0 = e0->halfedge(), h1=h0->next(), h2 = h1->next(),
         h3 = h0->twin(), h4 = h3->next(), h5 = h4->next(),
         h6 = h1->twin(), h7 = h2->twin(), h8 = h4->twin(),
         h9 = h5->twin();
    auto f0 = h0->face(), f1 = h3->face();
    // original vertices
    auto v1 = h3->vertex(), v2 = h0->vertex(),
         v3 = h6->vertex(), v4 = h5->vertex();
    auto new_vertex = newVertex();
    // add new edges
    auto e01 = newEdge(), e02 = newEdge(),
         e03 = newEdge(), e04 = newEdge();
    // add new halfedges
    auto ha = newHalfedge(), hb = newHalfedge(), hc = newHalfedge(),
         hd = newHalfedge(), he = newHalfedge(), hf = newHalfedge(),
         hg = newHalfedge(), hh = newHalfedge();
    // add new faces
    auto f01 = newFace(), f02 = newFace(),
         f11 = newFace(), f12 = newFace();
    // new_vetex's location and halfedge it belongs to
    new_vertex->position = (v1->position + v2->position) / 2;
    new_vertex->halfedge() = ha;
    // set new halfedges
    ha->setNeighbors(h1, he, new_vertex, e01, f01);
    hb->setNeighbors(ha, hc, v3, e03, f01);
    hc->setNeighbors(h2, hb, new_vertex, e03, f11);
    hd->setNeighbors(hc, hg, v2, e02, f11);
    he->setNeighbors(hf, ha, v1, e01, f02);
    hf->setNeighbors(h5, hh, new_vertex, e04, f02);
    hg->setNeighbors(h4, hd, new_vertex, e02, f12);
    hh->setNeighbors(hg, hf, v4, e04, f12);
    deleteHalfedge(h0);
    h1->setNeighbors(hb, h6, v1, h1->edge(), f01);
    h2->setNeighbors(hd, h7, v3, h2->edge(), f11);
    deleteHalfedge(h3);
    h4->setNeighbors(hh, h8, v2, h4->edge(), f12);
    h5->setNeighbors(he, h9, v4, h5->edge(), f02);
    h6->setNeighbors(h6->next(), h1, v3, h6->edge(), h6->face());
    h7->setNeighbors(h7->next(), h2, v2, h7->edge(), h7->face());
    h8->setNeighbors(h8->next(), h4, v4, h8->edge(), h8->face());
    h9->setNeighbors(h9->next(), h5, v1, h9->edge(), h9->face());
    // vertex assignments
    v1->halfedge() = h1;
    v2->halfedge() = hd;
    v3->halfedge() = hb;
    v4->halfedge() = hh;
    // edge assignments
    e01->halfedge() = ha;
    e02->halfedge() = hg;
    e03->halfedge() = hc;
    e04->halfedge() = hf;
    // face assignments
    f01->halfedge() = ha;
    f02->halfedge() = hf;
    f11->halfedge() = hc;
    f12->halfedge() = hg;
    // delete previous edge and face
    deleteEdge(e0);
    deleteFace(f0);
    deleteFace(f1);
    return new_vertex;
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
