#ifndef CGL_COLLADA_SPHEREINFO_H
#define CGL_COLLADA_SPHEREINFO_H

#include "collada_info.h"
#include "material_info.h"

namespace CGL { namespace Collada {

struct SphereInfo : Instance {
  float radius;						 ///< radius
  MaterialInfo* material;  ///< material of the sphere
}; // struct Sphere

std::ostream& operator<<(std::ostream& os, const SphereInfo& sphere);

} // namespace Collada
} // namespace CGL

#endif // CGL_COLLADA_SPHEREINFO_H
