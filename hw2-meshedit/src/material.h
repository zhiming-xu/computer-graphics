#ifndef CGL_MATERIAL_H
#define CGL_MATERIAL_H

#include "scene.h"
#include "texture.h"

namespace CGL {

  struct Material : Instance {

    Color emit; ///< emission color
    Color ambi; ///< ambient  color
    Color diff; ///< diffuse  color
    Color spec; ///< specular color

    float shininess; ///< shininess

    float refractive_index; ///< refractive index

    Texture* tex; ///< texture

  }; // struct Material

  std::ostream& operator<<( std::ostream& os, const Material& material );

} // namespace CGL

#endif // CGL_MATERIAL_H
