#ifndef CGL_LIGHT_H
#define CGL_LIGHT_H

#include "scene.h"

namespace CGL {

  typedef enum e_LightType {

    AMBIENT,
    POINT,
    DIRECTIONAL // FIXME : Add support for diffuse and specular lights.
    
  } LightType;

  struct Light : Instance {

    LightType light_type; ///< type of the light

    Color color;    ///< color of the light

    float attenuation;  ///< attentuation of the light

  }; // struct Light

  std::ostream& operator<<( std::ostream& os, const Light& light );

} // namespace CGL

#endif // CGL_LIGHT_H
