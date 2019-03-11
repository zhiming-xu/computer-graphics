#ifndef CGL_APPLICATION_H
#define CGL_APPLICATION_H

// STL
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

// libCGL
#include "CGL/CGL.h"
#include "CGL/renderer.h"
#include "CGL/osdtext.h"

// COLLADA
#include "collada/collada.h"
#include "collada/light_info.h"
#include "collada/sphere_info.h"
#include "collada/polymesh_info.h"
#include "collada/material_info.h"

// MeshEdit
#include "dynamic_scene/scene.h"
#include "halfEdgeMesh.h"
#include "meshEdit.h"

// PathTracer
#include "static_scene/scene.h"
#include "pathtracer.h"
#include "image.h"

// Shared modules
#include "camera.h"

using namespace std;

namespace CGL {

struct AppConfig {

  AppConfig () {

    pathtracer_ns_aa = 1;
    pathtracer_max_ray_depth = 1;
    pathtracer_ns_area_light = 1;

    pathtracer_ns_diff = 1;
    pathtracer_ns_glsy = 1;
    pathtracer_ns_refr = 1;

    pathtracer_num_threads = 1;
    pathtracer_envmap = NULL;

    pathtracer_samples_per_patch = 32;
    pathtracer_max_tolerance = 0.05f;
    pathtracer_direct_hemisphere_sample = false;

    pathtracer_filename = "";
  }

  size_t pathtracer_ns_aa;
  size_t pathtracer_max_ray_depth;
  size_t pathtracer_ns_area_light;

  size_t pathtracer_ns_diff;
  size_t pathtracer_ns_glsy;
  size_t pathtracer_ns_refr;

  size_t pathtracer_num_threads;
  HDRImageBuffer* pathtracer_envmap;

  float pathtracer_max_tolerance;
  size_t pathtracer_samples_per_patch;

  bool pathtracer_direct_hemisphere_sample;

  string pathtracer_filename;
};

class Application : public Renderer {
 public:

  Application(AppConfig config, bool gl = true);

  ~Application();

  void init();
  void render();
  void resize(size_t w, size_t h);

  std::string name();
  std::string info();

  void cursor_event( float x, float y );
  void scroll_event( float offset_x, float offset_y );
  void mouse_event( int key, int event, unsigned char mods );
  void keyboard_event( int key, int event, unsigned char mods  );

  void load(Collada::SceneInfo* sceneInfo);
  void render_to_file(std::string filename, size_t x, size_t y, size_t dx, size_t dy) { 
    set_up_pathtracer();
    pathtracer->render_to_file(filename, x, y, dx, dy); 
  }

  void load_camera(std::string filename) {
    camera.load_settings(filename);
  }

 private:

  enum Mode {
    EDIT_MODE,
    RENDER_MODE,
    VISUALIZE_MODE
  };
  Mode mode;

  void to_edit_mode();
  void set_up_pathtracer();

  DynamicScene::Scene *scene;
  PathTracer* pathtracer;

  // View Frustrum Variables.
  // On resize, the aspect ratio is changed. On reset_camera, the position and
  // orientation are reset but NOT the aspect ratio.
  Camera camera;
  Camera canonicalCamera;

  size_t screenW;
  size_t screenH;

  // Length of diagonal of bounding box for the mesh.
  // Guranteed to not have the camera occlude with the mes.
  double canonical_view_distance;

  // Rate of translation on scrolling.
  double scroll_rate;

  /*
    Called whenever the camera fov or screenW/screenH changes.
  */
  void set_projection_matrix();

  /**
   * Fills the DrawStyle structs.
   */
  void initialize_style();

  /**
   * Update draw styles properly given the current view distance.
   */
  void update_style();

  /**
   * Reads and combines the current modelview and projection matrices.
   */
  Matrix4x4 get_world_to_3DH();

  // Initialization functions to get the opengl cooking with oil.
  void init_camera(Collada::CameraInfo& camera, const Matrix4x4& transform);
  DynamicScene::SceneLight *init_light(Collada::LightInfo& light, const Matrix4x4& transform);
  DynamicScene::SceneObject *init_sphere(Collada::SphereInfo& polymesh, const Matrix4x4& transform);
  DynamicScene::SceneObject *init_polymesh(Collada::PolymeshInfo& polymesh, const Matrix4x4& transform);
  void init_material(Collada::MaterialInfo& material);

  void set_scroll_rate();

  // Resets the camera to the canonical initial view position.
  void reset_camera();

  // Rendering functions.
  void update_gl_camera();

  // style for elements that are neither hovered nor selected
  DynamicScene::DrawStyle defaultStyle;
  DynamicScene::DrawStyle hoverStyle;
  DynamicScene::DrawStyle selectStyle;

  // Internal event system //

  float mouseX, mouseY;
  enum e_mouse_button {
    LEFT   = MOUSE_LEFT,
    RIGHT  = MOUSE_RIGHT,
    MIDDLE = MOUSE_MIDDLE
  };

  bool leftDown;
  bool rightDown;
  bool middleDown;

  // Event handling //

  void mouse_pressed(e_mouse_button b);   // Mouse pressed.
  void mouse_released(e_mouse_button b);  // Mouse Released.
  void mouse1_dragged(float x, float y);  // Left Mouse Dragged.
  void mouse2_dragged(float x, float y);  // Right Mouse Dragged.
  void mouse_moved(float x, float y);     // Mouse Moved.

  // OSD text manager //
  OSDText textManager;
  Color text_color;
  vector<int> messages;

  // Coordinate System //
  bool show_coordinates;
  void draw_coordinates();

  // HUD //
  bool show_hud;
  void draw_hud();
  inline void draw_string(float x, float y,
    string str, size_t size, const Color& c);

  bool gl_window;

  std::string filename;

}; // class Application

} // namespace CGL

  #endif // CGL_APPLICATION_H
