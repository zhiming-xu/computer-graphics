#ifndef CGL_INTERSECT_H
#define CGL_INTERSECT_H

#include <vector>

#include "CGL/vector3D.h"
#include "CGL/spectrum.h"
#include "CGL/misc.h"

#include "bsdf.h"

namespace CGL { namespace StaticScene {

class Primitive;

/**
 * A record of an intersection point which includes the time of intersection
 * and other information needed for shading
 */
struct Intersection {

  Intersection() : t (INF_D), primitive(NULL), bsdf(NULL) { }

  double t;    ///< time of intersection

  const Primitive* primitive;  ///< the primitive intersected

  Vector3D n;  ///< normal at point of intersection

  BSDF* bsdf; ///< BSDF of the surface at point of intersection

  // More to follow.
};

} // namespace StaticScene
} // namespace CGL

#endif // CGL_INTERSECT_H
