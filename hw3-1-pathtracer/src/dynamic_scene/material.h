#ifndef CGL_DYNAMICSCENE_MATERIAL_H
#define CGL_DYNAMICSCENE_MATERIAL_H

#include "scene.h"
#include "../collada/material_info.h"

#include <iostream>

using std::cout;
using std::endl;

namespace CGL { namespace DynamicScene {

class Material {
 public:

  Material() {
    ambient = Color(0,0,0,0);
    diffuse = Color(0,0,0,0);
    specular = Color(0,0,0,0);
    shininess = 0;
  }

  Material(const Collada::MaterialInfo& info)
  : ambient(info.Ca),
    diffuse(info.Cd),
    specular(info.Cs),
    shininess(info.Ns) { }

  void set_material_properties() const {
    glBindTexture(GL_TEXTURE_2D, 0);

    GLfloat temp[4];

    temp[0] = ambient.r;
    temp[1] = ambient.b;
    temp[2] = ambient.g;
    temp[3] = 1.f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, temp);

    temp[0] = diffuse.r;
    temp[1] = diffuse.b;
    temp[2] = diffuse.g;
    temp[3] = 1.f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, temp);

    temp[0] = specular.r;
    temp[1] = specular.b;
    temp[2] = specular.g;
    temp[3] = 1.f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, temp);

    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  }

 private:
  Color ambient;      ///< Color - ambient
  Color diffuse;      ///< Color - diffuse
  Color specular;     ///< Color - specular

  float shininess;    ///< Numerical - shininess
};

} // namespace DynamicScene
} // namespace CGL

#endif //CGL_DYNAMICSCENE_MATERIAL_H
