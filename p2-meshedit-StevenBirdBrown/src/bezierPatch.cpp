#include "bezierPatch.h"
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace CGL {

  BezierPatch::BezierPatch()
  {
    controlPoints = std::vector< std::vector<Vector3D> >(4);
    for (int i = 0; i < 4; i++)
    {
      controlPoints[i] = std::vector<Vector3D>(4);
    }
  }

  void BezierPatch::add2mesh(Polymesh* mesh) const
  {
    int nu = 16;
    int nv = 16;
    for (int i = 0; i < nu; i++)
    {
      for (int j = 0; j < nv; j++)
      {
        Vector3D v0, v1, v2, v3;
        v0 = evaluate(i/double(nu), j/double(nv));
        v1 = evaluate((i+1)/double(nu), j/double(nv));
        v2 = evaluate((i+1)/double(nu), (j+1)/double(nv));
        v3 = evaluate(i/double(nu), (j+1)/double(nv));
        addTriangle(mesh, v1, v2, v0);
        addTriangle(mesh, v2, v3, v0);
      }
    }
  }

  void BezierPatch::addTriangle(Polymesh* mesh, const Vector3D& v0, const Vector3D& v1, const Vector3D& v2) const
  {
    size_t base = mesh->vertices.size();
    mesh->vertices.push_back(v0);
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    Polygon poly;
    poly.vertex_indices.push_back(base);
    poly.vertex_indices.push_back(base+1);
    poly.vertex_indices.push_back(base+2);
    mesh->polygons.push_back(poly);
  }

  void BezierPatch::loadControlPoints(FILE* file)
  {
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<4; j++)
      {
        fscanf(file, "%lf %lf %lf", &controlPoints[i][j].x, &controlPoints[i][j].y,
        &controlPoints[i][j].z);
      }
    }
  }

}
