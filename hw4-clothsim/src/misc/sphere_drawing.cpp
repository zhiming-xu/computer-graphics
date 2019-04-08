#include <cmath>
#include <nanogui/nanogui.h>

#include "sphere_drawing.h"

#include "CGL/color.h"
#include "CGL/vector3D.h"

#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5
#define TANGEN_OFFSET 8
#define VERTEX_SIZE 11

using namespace nanogui;

namespace CGL {
namespace Misc {

SphereMesh::SphereMesh(int num_lat, int num_lon)
: sphere_num_lat(num_lat)
, sphere_num_lon(num_lon)
, sphere_num_vertices((sphere_num_lat + 1) * (sphere_num_lon + 1))
, sphere_num_indices(6 * sphere_num_lat * sphere_num_lon) {
  
  Indices.resize(sphere_num_indices);
  Vertices.resize(VERTEX_SIZE * sphere_num_vertices);
  
  
  for (int i = 0; i <= sphere_num_lat; i++) {
    for (int j = 0; j <= sphere_num_lon; j++) {
      double lat = ((double)i) / sphere_num_lat;
      double lon = ((double)j) / sphere_num_lon;
      double *vptr = &Vertices[VERTEX_SIZE * s_index(i, j)];

      double uv_u = lon;
      double uv_v = lat;

      vptr[TCOORD_OFFSET + 0] = uv_u;
      vptr[TCOORD_OFFSET + 1] = uv_v;
      
      // Simple patch to rotate the sphere so by default
      // the seam is facing away from the camera
      lon += 0.5;
      if (lon >= 1.0) {
        lon -= 1.0;
      }

      lat *= M_PI;
      lon *= 2 * M_PI;

      // Vertex and normals are actually the same here
      vptr[NORMAL_OFFSET + 0] = vptr[VERTEX_OFFSET + 0] = sin(lat) * sin(lon);
      vptr[NORMAL_OFFSET + 1] = vptr[VERTEX_OFFSET + 1] = cos(lat);
      vptr[NORMAL_OFFSET + 2] = vptr[VERTEX_OFFSET + 2] = sin(lat) * cos(lon);
      
      // Compute tangents (take partial derivative with respect to longitude, normalize)
      vptr[TANGEN_OFFSET + 0] = cos(lon);
      vptr[TANGEN_OFFSET + 1] = 0;
      vptr[TANGEN_OFFSET + 2] = -sin(lon);
    }
  }

  for (int i = 0; i < sphere_num_lat; i++) {
    for (int j = 0; j < sphere_num_lon; j++) {
      unsigned int *iptr = &Indices[6 * (sphere_num_lon * i + j)];

      unsigned int i00 = s_index(i, j);
      unsigned int i10 = s_index(i + 1, j);
      unsigned int i11 = s_index(i + 1, j + 1);
      unsigned int i01 = s_index(i, j + 1);

      iptr[0] = i00;
      iptr[1] = i10;
      iptr[2] = i11;
      iptr[3] = i11;
      iptr[4] = i01;
      iptr[5] = i00;
    }
  }
  
  build_data();
}

int SphereMesh::s_index(int x, int y) {
  return ((x) * (sphere_num_lon + 1) + (y));
}

void SphereMesh::build_data() {
  
  positions = MatrixXf(4, sphere_num_indices * 3);
  normals = MatrixXf(4, sphere_num_indices * 3);
  uvs = MatrixXf(2, sphere_num_indices * 3);
  tangents = MatrixXf(4, sphere_num_indices * 3);

  for (int i = 0; i < sphere_num_indices; i += 3) {
    double *vPtr1 = &Vertices[VERTEX_SIZE * Indices[i]];
    double *vPtr2 = &Vertices[VERTEX_SIZE * Indices[i + 1]];
    double *vPtr3 = &Vertices[VERTEX_SIZE * Indices[i + 2]];

    Vector3D p1(vPtr1[VERTEX_OFFSET], vPtr1[VERTEX_OFFSET + 1],
                vPtr1[VERTEX_OFFSET + 2]);
    Vector3D p2(vPtr2[VERTEX_OFFSET], vPtr2[VERTEX_OFFSET + 1],
                vPtr2[VERTEX_OFFSET + 2]);
    Vector3D p3(vPtr3[VERTEX_OFFSET], vPtr3[VERTEX_OFFSET + 1],
                vPtr3[VERTEX_OFFSET + 2]);

    Vector3D n1(vPtr1[NORMAL_OFFSET], vPtr1[NORMAL_OFFSET + 1],
                vPtr1[NORMAL_OFFSET + 2]);
    Vector3D n2(vPtr2[NORMAL_OFFSET], vPtr2[NORMAL_OFFSET + 1],
                vPtr2[NORMAL_OFFSET + 2]);
    Vector3D n3(vPtr3[NORMAL_OFFSET], vPtr3[NORMAL_OFFSET + 1],
                vPtr3[NORMAL_OFFSET + 2]);
        
    Vector3D uv1(vPtr1[TCOORD_OFFSET], vPtr1[TCOORD_OFFSET + 1], 0);
    Vector3D uv2(vPtr2[TCOORD_OFFSET], vPtr2[TCOORD_OFFSET + 1], 0);
    Vector3D uv3(vPtr3[TCOORD_OFFSET], vPtr3[TCOORD_OFFSET + 1], 0);
    
    Vector3D t1(vPtr1[TANGEN_OFFSET], vPtr1[TANGEN_OFFSET + 1],
                vPtr1[TANGEN_OFFSET + 2]);
    Vector3D t2(vPtr2[TANGEN_OFFSET], vPtr2[TANGEN_OFFSET + 1],
                vPtr2[TANGEN_OFFSET + 2]);
    Vector3D t3(vPtr3[TANGEN_OFFSET], vPtr3[TANGEN_OFFSET + 1],
                vPtr3[TANGEN_OFFSET + 2]);

    positions.col(i    ) << p1.x, p1.y, p1.z, 1.0;
    positions.col(i + 1) << p2.x, p2.y, p2.z, 1.0;
    positions.col(i + 2) << p3.x, p3.y, p3.z, 1.0;

    normals.col(i    ) << n1.x, n1.y, n1.z, 0.0;
    normals.col(i + 1) << n2.x, n2.y, n2.z, 0.0;
    normals.col(i + 2) << n3.x, n3.y, n3.z, 0.0;
    
    uvs.col(i    ) << uv1.x, uv1.y;
    uvs.col(i + 1) << uv2.x, uv2.y;
    uvs.col(i + 2) << uv3.x, uv3.y;
    
    tangents.col(i    ) << t1.x, t1.y, t1.z, 0.0;
    tangents.col(i + 1) << t2.x, t2.y, t2.z, 0.0;
    tangents.col(i + 2) << t3.x, t3.y, t3.z, 0.0;
  }
}

void SphereMesh::draw_sphere(GLShader &shader, const Vector3D &p, double r) {

  Matrix4f model;
  model << r, 0, 0, p.x, 0, r, 0, p.y, 0, 0, r, p.z, 0, 0, 0, 1;

  shader.setUniform("u_model", model);


  shader.uploadAttrib("in_position", positions);
  if (shader.attrib("in_normal", false) != -1) {
    shader.uploadAttrib("in_normal", normals);
  }
  if (shader.attrib("in_uv", false) != -1) {
    shader.uploadAttrib("in_uv", uvs);
  }
  if (shader.attrib("in_tangent", false) != -1) {
    shader.uploadAttrib("in_tangent", tangents, false);
  }

  shader.drawArray(GL_TRIANGLES, 0, sphere_num_indices);
}

} // namespace Misc
} // namespace CGL
