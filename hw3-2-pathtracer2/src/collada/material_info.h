#ifndef CGL_COLLADA_MATERIALINFO_H
#define CGL_COLLADA_MATERIALINFO_H

#include "CGL/color.h"
#include "collada_info.h"
#include "../bsdf.h"

namespace CGL { namespace Collada {

struct MaterialInfo : public Instance {

  BSDF* bsdf;
  
//   Texture* tex; ///< texture

}; // struct Material

std::ostream& operator<<(std::ostream& os, const MaterialInfo& material);

} // namespace Collada
} // namespace CGL

#endif // CGL_COLLADA_MATERIALINFO_H
