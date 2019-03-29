//
// TODO: Copy over 3-1 code after turning on BUILD_3-1 flag
//

#include "part1_code.h"
#include <time.h>

using namespace CGL::StaticScene;

using std::min;
using std::max;

namespace CGL {

  Spectrum PathTracer::estimate_direct_lighting_hemisphere(const Ray& r, const Intersection& isect) {

  }

  Spectrum PathTracer::estimate_direct_lighting_importance(const Ray& r, const Intersection& isect) {

  }

  Spectrum PathTracer::zero_bounce_radiance(const Ray&r, const Intersection& isect) {

  }

  Spectrum PathTracer::one_bounce_radiance(const Ray&r, const Intersection& isect) {

  }

  Spectrum PathTracer::at_least_one_bounce_radiance(const Ray&r, const Intersection& isect) {

  }

  Spectrum PathTracer::est_radiance_global_illumination(const Ray &r) {

  }

  Spectrum PathTracer::raytrace_pixel(size_t x, size_t y, bool useThinLens) {

  }

  // Diffuse BSDF //

  Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {

  }

  Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  }

  // Camera //
  Ray Camera::generate_ray(double x, double y) const {

  }
}
