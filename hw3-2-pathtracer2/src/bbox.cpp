#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  Vector3D submin = this->min - r.o;
  Vector3D submax = this->max - r.o;
  auto ret = true;
  double min_time_x = std::min(submin.x / r.d.x, submax.x / r.d.x);
  double max_time_x = std::max(submin.x / r.d.x, submax.x / r.d.x);
  double min_time_y = std::min(submin.y / r.d.y, submax.y / r.d.y);
  double max_time_y = std::max(submin.y / r.d.y, submax.y / r.d.y);
  double min_time_z = std::min(submin.z / r.d.z, submax.z / r.d.z);
  double max_time_z = std::max(submin.z / r.d.z, submax.z / r.d.z);
  double min_time_all = std::max(min_time_x, std::max(min_time_y, min_time_z));
  double max_time_all = std::min(max_time_x, std::min(max_time_y, max_time_z));
  if (min_time_all > max_time_all || min_time_all > r.max_t || max_time_all < r.min_t)
  {
    ret = false;
  }
  t0 = min_time_all;
  t1 = max_time_all;
  return ret;
}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
