#ifndef CGL_DYNAMICSCENE_POINTLIGHT_H
#define CGL_DYNAMICSCENE_POINTLIGHT_H

#include "scene.h"
#include "../static_scene/light.h"

using std::cout;
using std::endl;

namespace CGL { namespace DynamicScene {

class PointLight : public SceneLight {
 public:

  PointLight(const Collada::LightInfo& light_info, 
             const Matrix4x4& transform) {    

    this->spectrum = light_info.spectrum;
    this->position = (transform * Vector4D(light_info.position, 1)).to3D();
  }

  StaticScene::SceneLight *get_static_light() const {
    StaticScene::PointLight *l = 
      new StaticScene::PointLight(spectrum, position);
    return l;
  }

 private:

  Spectrum spectrum;
  Vector3D position;

};

} // namespace DynamicScene
} // namespace CGL

#endif //CGL_DYNAMICSCENE_POINTLIGHT_H
