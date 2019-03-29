#ifndef CGL_DYNAMICSCENE_ENVIRONMENTLIGHT_H
#define CGL_DYNAMICSCENE_ENVIRONMENTLIGHT_H

#include "scene.h"
#include "../image.h"
#include "../static_scene/light.h"

namespace CGL { namespace DynamicScene {

class EnvironmentLight : public SceneLight {
 public:

  EnvironmentLight(HDRImageBuffer* envmap) : envmap(envmap) { }

  StaticScene::SceneLight *get_static_light() const {
    StaticScene::EnvironmentLight* l = 
      new StaticScene::EnvironmentLight(envmap);
    return l;
  }

 private:

  HDRImageBuffer* envmap;

};

} // namespace DynamicScene
} // namespace CGL

#endif // CGL_DYNAMICSCENE_ENVIRONMENTLIGHT_H
