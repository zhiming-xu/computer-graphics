#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  auto a = dot(r.d, r.d), b = 2 * dot((r.o - this->o), r.d), c = dot(r.o - this->o, r.o - this->o) - this->r2;
  auto delta = b * b - 4 * a * c;
  auto ret = false;
  if(delta <= 0)
       ret = false;
  else
  {
    auto tmp1 = (-b - sqrt(delta)) / 2 / a;
    auto tmp2 = (-b + sqrt(delta)) / 2 / a;
    if(tmp1 >= r.min_t && tmp2 <= r.max_t)
    {
      t1 = tmp1;
      t2 = tmp2;
      r.max_t = t1>0?t1:t2;
      ret = true;
    }
  }
  return ret;
}

bool Sphere::intersect(const Ray& r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1, t2;
  return this->test(r, t1, t2);
}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1, t2;
  auto ret = this->test(r, t1, t2);
  if(!ret)
    return ret;
  i->bsdf = get_bsdf();
  i->primitive = this;
  i->t = r.max_t;
  Vector3D norm = (r.max_t * r.d + r.o) - this->o;
  norm.normalize();
  i->n = norm;
  return ret;

}

void Sphere::draw(const Color& c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c, float alpha) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
