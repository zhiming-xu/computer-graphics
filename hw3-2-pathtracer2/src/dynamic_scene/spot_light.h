#ifndef CGL_DYNAMICSCENE_SPOTLIGHT_H
#define CGL_DYNAMICSCENE_SPOTLIGHT_H

#include "scene.h"
#include "../static_scene/light.h"

namespace CGL { namespace DynamicScene {

class SpotLight : public SceneLight {
 public:

  SpotLight(const Collada::LightInfo& light_info, 
           const Matrix4x4& transform) {

    this->spectrum = light_info.spectrum;
    this->position = (transform * Vector4D(light_info.position, 1)).to3D();
    this->direction = (transform * Vector4D(light_info.direction, 1)).to3D() - position;
    this->direction.normalize();
  }

  StaticScene::SceneLight *get_static_light() const {
    StaticScene::SpotLight* l = 
      new StaticScene::SpotLight(spectrum, position, direction, PI * .5f);
    return l;
  }

 private:

  Spectrum spectrum;
  Vector3D direction;
  Vector3D position;

};

} // namespace DynamicScene
} // namespace CGL

#endif //CGL_DYNAMICSCENE_SPOTLIGHT_H
