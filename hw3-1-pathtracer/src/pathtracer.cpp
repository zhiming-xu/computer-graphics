#include "pathtracer.h"
#include "bsdf.h"
#include "ray.h"

#include <stack>
#include <random>
#include <algorithm>
#include <sstream>

#include "CGL/CGL.h"
#include "CGL/vector3D.h"
#include "CGL/matrix3x3.h"
#include "CGL/lodepng.h"

#include "GL/glew.h"

#include "static_scene/sphere.h"
#include "static_scene/triangle.h"
#include "static_scene/light.h"

using namespace CGL::StaticScene;

using std::min;
using std::max;

namespace CGL {

PathTracer::PathTracer(size_t ns_aa,
                       size_t max_ray_depth, size_t ns_area_light,
                       size_t ns_diff, size_t ns_glsy, size_t ns_refr,
                       size_t num_threads,
                       size_t samples_per_batch,
                       float max_tolerance,
                       HDRImageBuffer* envmap,
                       bool direct_hemisphere_sample,
                       string filename) {
  state = INIT,
  this->ns_aa = ns_aa;
  this->max_ray_depth = max_ray_depth;
  this->ns_area_light = ns_area_light;
  this->ns_diff = ns_diff;
  this->ns_glsy = ns_diff;
  this->ns_refr = ns_refr;
  this->samplesPerBatch = samples_per_batch;
  this->maxTolerance = max_tolerance;
  this->direct_hemisphere_sample = direct_hemisphere_sample;
  this->filename = filename;

  if (envmap) {
    this->envLight = new EnvironmentLight(envmap);
  } else {
    this->envLight = NULL;
  }

  bvh = NULL;
  scene = NULL;
  camera = NULL;

  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  show_rays = true;

  imageTileSize = 32;
  numWorkerThreads = num_threads;
  workerThreads.resize(numWorkerThreads);

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;

}

PathTracer::~PathTracer() {

  delete bvh;
  delete gridSampler;
  delete hemisphereSampler;

}

void PathTracer::set_scene(Scene *scene) {

  if (state != INIT) {
    return;
  }

  if (this->scene != nullptr) {
    delete scene;
    delete bvh;
    selectionHistory.pop();
  }

  if (this->envLight != nullptr) {
    scene->lights.push_back(this->envLight);
  }

  this->scene = scene;
  build_accel();

  if (has_valid_configuration()) {
    state = READY;
  }
}

void PathTracer::set_camera(Camera *camera) {

  if (state != INIT) {
    return;
  }

  this->camera = camera;
  if (has_valid_configuration()) {
    state = READY;
  }

}

void PathTracer::set_frame_size(size_t width, size_t height) {
  if (state != INIT && state != READY) {
    stop();
  }
  sampleBuffer.resize(width, height);
  frameBuffer.resize(width, height);
  cell_tl = Vector2D(0,0); 
  cell_br = Vector2D(width, height);
  render_cell = false;
  sampleCountBuffer.resize(width * height);
  if (has_valid_configuration()) {
    state = READY;
  }
}

bool PathTracer::has_valid_configuration() {
  return scene && camera && gridSampler && hemisphereSampler &&
         (!sampleBuffer.is_empty());
}

void PathTracer::update_screen() {
  switch (state) {
    case INIT:
    case READY:
      break;
    case VISUALIZE:
      visualize_accel();
      break;
    case RENDERING:
      glDrawPixels(frameBuffer.w, frameBuffer.h, GL_RGBA,
                   GL_UNSIGNED_BYTE, &frameBuffer.data[0]);
      if (render_cell)
        visualize_cell();
      break;
    case DONE:
        //sampleBuffer.tonemap(frameBuffer, tm_gamma, tm_level, tm_key, tm_wht);
      glDrawPixels(frameBuffer.w, frameBuffer.h, GL_RGBA,
                   GL_UNSIGNED_BYTE, &frameBuffer.data[0]);
      if (render_cell)
        visualize_cell();
      break;
  }
}

void PathTracer::stop() {
  switch (state) {
    case INIT:
    case READY:
      break;
    case VISUALIZE:
      while (selectionHistory.size() > 1) {
        selectionHistory.pop();
      }
      state = READY;
      break;
    case RENDERING:
      continueRaytracing = false;
    case DONE:
      for (int i=0; i<numWorkerThreads; i++) {
            workerThreads[i]->join();
            delete workerThreads[i];
        }
      state = READY;
      break;
  }
}

void PathTracer::clear() {
  if (state != READY) return;
  delete bvh;
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  selectionHistory.pop();
  sampleBuffer.resize(0, 0);
  frameBuffer.resize(0, 0);
  state = INIT;
  render_cell = false;
}

void PathTracer::start_visualizing() {
  if (state != READY) {
    return;
  }
  state = VISUALIZE;
}

void PathTracer::start_raytracing() {
  if (state != READY) return;

  rayLog.clear();
  workQueue.clear();

  state = RENDERING;
  continueRaytracing = true;
  workerDoneCount = 0;

  sampleBuffer.clear();
  if (!render_cell) {
    frameBuffer.clear();
    num_tiles_w = sampleBuffer.w / imageTileSize + 1;
    num_tiles_h = sampleBuffer.h / imageTileSize + 1;
    tilesTotal = num_tiles_w * num_tiles_h;
    tilesDone = 0;
    tile_samples.resize(num_tiles_w * num_tiles_h);
    memset(&tile_samples[0], 0, num_tiles_w * num_tiles_h * sizeof(int));

    // populate the tile work queue
    for (size_t y = 0; y < sampleBuffer.h; y += imageTileSize) {
        for (size_t x = 0; x < sampleBuffer.w; x += imageTileSize) {
            workQueue.put_work(WorkItem(x, y, imageTileSize, imageTileSize));
        }
    }
  } else {
    int w = (cell_br-cell_tl).x;
    int h = (cell_br-cell_tl).y;
    int imTS = imageTileSize / 4;
    num_tiles_w = w / imTS + 1;
    num_tiles_h = h / imTS + 1;
    tilesTotal = num_tiles_w * num_tiles_h;
    tilesDone = 0;
    tile_samples.resize(num_tiles_w * num_tiles_h);
    memset(&tile_samples[0], 0, num_tiles_w * num_tiles_h * sizeof(int));

    // populate the tile work queue
    for (size_t y = cell_tl.y; y < cell_br.y; y += imTS) {
      for (size_t x = cell_tl.x; x < cell_br.x; x += imTS) {
        workQueue.put_work(WorkItem(x, y, 
          min(imTS, (int)(cell_br.x-x)), min(imTS, (int)(cell_br.y-y)) ));
      }
    }
  }

  bvh->total_isects = 0; bvh->total_rays = 0;
  // launch threads
  fprintf(stdout, "[PathTracer] Rendering... "); fflush(stdout);
  for (int i=0; i<numWorkerThreads; i++) {
      workerThreads[i] = new std::thread(&PathTracer::worker_thread, this);
  }
}

void PathTracer::render_to_file(string filename, size_t x, size_t y, size_t dx, size_t dy) {
  if (x == -1) {
    unique_lock<std::mutex> lk(m_done);
    start_raytracing();
    cv_done.wait(lk, [this]{ return state == DONE; });
    lk.unlock();
    save_image(filename);
    fprintf(stdout, "[PathTracer] Job completed.\n");
  } else {
    render_cell = true;
    cell_tl = Vector2D(x,y);
    cell_br = Vector2D(x+dx,y+dy);
    ImageBuffer buffer;
    raytrace_cell(buffer);
    save_image(filename, &buffer);
    fprintf(stdout, "[PathTracer] Cell job completed.\n");
  }
}


void PathTracer::build_accel() {

  // collect primitives //
  fprintf(stdout, "[PathTracer] Collecting primitives... "); fflush(stdout);
  timer.start();
  vector<Primitive *> primitives;
  for (SceneObject *obj : scene->objects) {
    const vector<Primitive *> &obj_prims = obj->get_primitives();
    primitives.reserve(primitives.size() + obj_prims.size());
    primitives.insert(primitives.end(), obj_prims.begin(), obj_prims.end());
  }
  timer.stop();
  fprintf(stdout, "Done! (%.4f sec)\n", timer.duration());

  // build BVH //
  fprintf(stdout, "[PathTracer] Building BVH from %lu primitives... ", primitives.size()); 
  fflush(stdout);
  timer.start();
  bvh = new BVHAccel(primitives);
  timer.stop();
  fprintf(stdout, "Done! (%.4f sec)\n", timer.duration());

  // initial visualization //
  selectionHistory.push(bvh->get_root());
}

void PathTracer::visualize_accel() const {

  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glLineWidth(1);
  glEnable(GL_DEPTH_TEST);

  // hardcoded color settings
  Color cnode = Color(.5, .5, .5); float cnode_alpha = 0.25f;
  Color cnode_hl = Color(1., .25, .0); float cnode_hl_alpha = 0.6f;
  Color cnode_hl_child = Color(1., 1., 1.); float cnode_hl_child_alpha = 0.6f;

  Color cprim_hl_left = Color(.6, .6, 1.); float cprim_hl_left_alpha = 1.f;
  Color cprim_hl_right = Color(.8, .8, 1.); float cprim_hl_right_alpha = 1.f;
  Color cprim_hl_edges = Color(0., 0., 0.); float cprim_hl_edges_alpha = 0.5f;

  BVHNode *selected = selectionHistory.top();

  // render solid geometry (with depth offset)
  glPolygonOffset(1.0, 1.0);
  glEnable(GL_POLYGON_OFFSET_FILL);

  if (selected->isLeaf()) {
    bvh->draw(selected, cprim_hl_left, cprim_hl_left_alpha);
  } else {
    bvh->draw(selected->l, cprim_hl_left, cprim_hl_left_alpha);
    bvh->draw(selected->r, cprim_hl_right, cprim_hl_right_alpha);
  }

  glDisable(GL_POLYGON_OFFSET_FILL);

  // draw geometry outline
  bvh->drawOutline(selected, cprim_hl_edges, cprim_hl_edges_alpha);

  // keep depth buffer check enabled so that mesh occluded bboxes, but
  // disable depth write so that bboxes don't occlude each other.
  glDepthMask(GL_FALSE);

  // create traversal stack
  stack<BVHNode *> tstack;

  // push initial traversal data
  tstack.push(bvh->get_root());

  // draw all BVH bboxes with non-highlighted color
  while (!tstack.empty()) {

    BVHNode *current = tstack.top();
    tstack.pop();

    current->bb.draw(cnode, cnode_alpha);
    if (current->l) tstack.push(current->l);
    if (current->r) tstack.push(current->r);
  }

  // draw selected node bbox and primitives
  if (selected->l) selected->l->bb.draw(cnode_hl_child, cnode_hl_child_alpha);
  if (selected->r) selected->r->bb.draw(cnode_hl_child, cnode_hl_child_alpha);

  glLineWidth(3.f);
  selected->bb.draw(cnode_hl, cnode_hl_alpha);

  // now perform visualization of the rays
  if (show_rays) {
      glLineWidth(1.f);
      glBegin(GL_LINES);

      for (size_t i=0; i<rayLog.size(); i+=500) {

          const static double VERY_LONG = 10e4;
          double ray_t = VERY_LONG;

          // color rays that are hits yellow
          // and rays this miss all geometry red
          if (rayLog[i].hit_t >= 0.0) {
              ray_t = rayLog[i].hit_t;
              glColor4f(1.f, 1.f, 0.f, 0.1f);
          } else {
              glColor4f(1.f, 0.f, 0.f, 0.1f);
          }

          Vector3D end = rayLog[i].o + ray_t * rayLog[i].d;

          glVertex3f(rayLog[i].o[0], rayLog[i].o[1], rayLog[i].o[2]);
          glVertex3f(end[0], end[1], end[2]);
      }
      glEnd();
  }

  glDepthMask(GL_TRUE);
  glPopAttrib();
}

void PathTracer::visualize_cell() const {
  glPushAttrib(GL_VIEWPORT_BIT);
  glViewport(0, 0, sampleBuffer.w, sampleBuffer.h);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, sampleBuffer.w, sampleBuffer.h, 0, 0, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0, 0, -1);

  glColor4f(1.0, 0.0, 0.0, 0.8);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // Draw the Red Rectangle.
  glBegin(GL_LINE_LOOP);
  glVertex2f(cell_tl.x, sampleBuffer.h-cell_br.y);
  glVertex2f(cell_br.x, sampleBuffer.h-cell_br.y);
  glVertex2f(cell_br.x, sampleBuffer.h-cell_tl.y);
  glVertex2f(cell_tl.x, sampleBuffer.h-cell_tl.y);
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();

  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
}

void PathTracer::key_press(int key) {
  BVHNode *current = selectionHistory.top();
  switch (key) {
  case ']':
      ns_aa *=2;
      fprintf(stdout, "[PathTracer] Samples per pixel changed to %lu\n", ns_aa);
      //tm_key = clamp(tm_key + 0.02f, 0.0f, 1.0f);
      break;
  case '[':
      //tm_key = clamp(tm_key - 0.02f, 0.0f, 1.0f);
      ns_aa /=2;
      if (ns_aa < 1) ns_aa = 1;
      fprintf(stdout, "[PathTracer] Samples per pixel changed to %lu\n", ns_aa);
      break;
  case '=': case '+':
      ns_area_light *= 2;
      fprintf(stdout, "[PathTracer] Area light sample count increased to %zu.\n", ns_area_light);
      break;
  case '-': case '_':
      if (ns_area_light > 1) ns_area_light /= 2;
      fprintf(stdout, "[PathTracer] Area light sample count decreased to %zu.\n", ns_area_light);
      break;
  case '.': case '>':
      max_ray_depth++;
      fprintf(stdout, "[PathTracer] Max ray depth increased to %zu.\n", max_ray_depth);
      break;
  case ',': case '<':
      if (max_ray_depth) max_ray_depth--;
      fprintf(stdout, "[PathTracer] Max ray depth decreased to %zu.\n", max_ray_depth);
      break;
  case 'h': case 'H':
      direct_hemisphere_sample = !direct_hemisphere_sample;
      fprintf(stdout, "[PathTracer] Toggled direct lighting to %s\n", (direct_hemisphere_sample ? "uniform hemisphere sampling" : "importance light sampling"));
      break;
  case KEYBOARD_UP:
      if (current != bvh->get_root()) {
          selectionHistory.pop();
      }
      break;
  case KEYBOARD_LEFT:
      if (current->l) {
          selectionHistory.push(current->l);
      }
      break;
  case KEYBOARD_RIGHT:
      if (current->l) {
          selectionHistory.push(current->r);
      }
      break;

  case 'C':
    render_cell = !render_cell;
    if (render_cell)
      fprintf(stdout, "[PathTracer] Now in cell render mode.\n");
    else
      fprintf(stdout, "[PathTracer] No longer in cell render mode.\n");
    break;

  case 'a': case 'A':
      show_rays = !show_rays;
  default:
      return;
  }
}


Spectrum PathTracer::estimate_direct_lighting_hemisphere(const Ray& r, const Intersection& isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere. 

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.

  // We store the transformation from 'local object space' to 'world space' in the matrix o2w
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // a local coordinate space for the object hit point
  const Vector3D& hit_p = r.o + r.d * isect.t;
  // w_out points towards the source of the ray (e.g.,toward the camera if this is a primary ray)
  // Remember that this should be opposite to the direction that the ray was traveling.
  const Vector3D& w_out = w2o * (-r.d);

  // This is the same number of total samples as estimate_direct_lighting_importance (outside of delta lights). 
  // We keep the same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Spectrum L_out;

  // TODO (Part 3.2): 
  // Write your sampling loop here
  // COMMENT OUT `normal_shading` IN `est_radiance_global_illumination` BEFORE YOU BEGIN
  for (int i = 0; i < num_samples; i++) {
    //direction in world space
    Vector3D w_in = hemisphereSampler->get_sample();

    Vector3D raydir = o2w * w_in;
    Vector3D rayorig = hit_p + (EPS_D * raydir); //If you don't do this, the ray will frequently intersect the ray's origin triangle at the same spot again because of floating point imprecision.

    Ray r = Ray(rayorig, raydir);
    Intersection newIsect;
    bool inter = bvh->intersect(r, &newIsect);

    if (inter) {
      //bsdf at point
      Spectrum b = isect.bsdf->f(w_out, w_in);
      //incoming radiance
      Spectrum e = newIsect.bsdf->get_emission();
      //w_in.z is cosine of angle between w_in and normal vector
      L_out += b * e * cos_theta(w_in);
    }
  }

  L_out = (L_out * 2.0 * PI) / (1.0 * num_samples);

  return L_out;
}

Spectrum PathTracer::estimate_direct_lighting_importance(const Ray& r, const Intersection& isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // To implement importance sampling, sample only from lights, not uniformly in a hemisphere. 

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D& hit_p = r.o + r.d * isect.t;
  const Vector3D& w_out = w2o * (-r.d);
  Spectrum L_out;

  // TODO (Part 3.2): 
  // Here is where your code for looping over scene lights goes
  // COMMENT OUT `normal_shading` IN `est_radiance_global_illumination` BEFORE YOU BEGIN


  for (SceneLight* light : scene->lights) {
    int num_samples;
    if (light->is_delta_light()) num_samples = 1;
    else num_samples = ns_area_light;

    Vector3D wi;
    float distToLight;
    float pdf;

    for (int i = 0; i < num_samples; i++) {
        Spectrum radiance = light->sample_L(hit_p, &wi, &distToLight, &pdf);

        // In order to pass it to the BSDF, you need to compute it in object space
        Vector3D w_in = w2o * wi;

        if (w_in.z >= 0) {
            Vector3D rayorig = hit_p + (EPS_D * wi);
            Ray r2 = Ray(rayorig, wi);
            r2.max_t = distToLight;

            Intersection newIsect;

            bool inter = bvh->intersect(r2, &newIsect);

            if (!inter) {
                Spectrum b = isect.bsdf->f(w_out, w_in);
                L_out += (radiance * b * cos_theta(w_in)) / pdf;
            }
        }
    }
    L_out /= (1.0 * num_samples);
  }
  return L_out;
}

Spectrum PathTracer::zero_bounce_radiance(const Ray&r, const Intersection& isect) {

  // TODO (Part 4.2):
  // Returns the light that results from no bounces of light

  return isect.bsdf->get_emission();

}

Spectrum PathTracer::one_bounce_radiance(const Ray&r, const Intersection& isect) {
  
  // TODO (Part 4.2):
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`
  // (you implemented these functions in Part 3)

  
  if (direct_hemisphere_sample)
    return estimate_direct_lighting_hemisphere(r, isect);
  else
    return estimate_direct_lighting_importance(r, isect);
}

Spectrum PathTracer::at_least_one_bounce_radiance(const Ray&r, const Intersection& isect) {
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);

  Spectrum L_out = one_bounce_radiance(r, isect);

  // TODO (Part 4.2): 
  // Here is where your code for sampling the BSDF,
  // performing Russian roulette step, and returning a recursively 
  // traced ray (when applicable) goes

  float russianRoulette = 0.7;
  Vector3D w_in;
  float pdf;
  Spectrum sample = isect.bsdf->sample_f(w_out, &w_in, &pdf);
  bool t = (coin_flip(1 - russianRoulette) && (max_ray_depth <= 1 || r.depth != max_ray_depth)) || (r.depth <= 1);
  if (!t) {
    Vector3D world_wi = o2w * w_in;
    Ray newR((EPS_D * world_wi) + hit_p, world_wi, INF_D, r.depth - 1);
    Intersection newIsect;
    if (bvh->intersect(newR, &newIsect)) {
      Spectrum bounce = at_least_one_bounce_radiance(newR, newIsect);
      if (r.depth == max_ray_depth) L_out += (w_in.z * sample * bounce)/pdf;
      else L_out += (w_in.z * sample * bounce)/pdf/russianRoulette;
    }
  }
  return L_out;
}

Spectrum PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Spectrum L_out;

  // You will extend this in assignment 3-2. 
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  if (!bvh->intersect(r, &isect)) 
    return L_out;

  // This line returns a color depending only on the normal vector 
  // to the surface at the intersection point.
  // REMOVE IT when you are ready to begin Part 3.

  // return normal_shading(isect.n);

  // TODO (Part 3): Return the direct illumination.

  // L_out = estimate_direct_lighting_hemisphere(r, isect);
  // L_out = estimate_direct_lighting_importance(r, isect);


  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct

  return zero_bounce_radiance(r, isect) + at_least_one_bounce_radiance(r, isect);

}

Spectrum PathTracer::raytrace_pixel(size_t x, size_t y) {

  // TODO (Part 1.1):
  // Make a loop that generates num_samples camera rays and traces them 
  // through the scene. Return the average Spectrum. 
  // You should call est_radiance_global_illumination in this function.

  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

  int num_samples = ns_aa;            // total samples to evaluate
  Vector2D origin = Vector2D(x,y);    // bottom left corner of the pixel

  double width = sampleBuffer.w;
  double height = sampleBuffer.h;
  // double s1 = 0;
  // double s2 = 0;
  if (num_samples == 1) {
    Ray ray = camera->generate_ray((x + 0.5)/width, (y + 0.5)/height);
    ray.depth = max_ray_depth;
    return est_radiance_global_illumination(ray);
  } 
  else {
    Spectrum spect = Spectrum();
    int i = 0;
    for (; i < num_samples; i++) {
      // if (i % samplesPerBatch == 0 && i > 1) {
      //   double mean = s1/double(i);
      //   double varSquare = (s2 - (s1*s1)/double(i))/(i - 1.0);
      //   if (1.96*sqrt(varSquare/double(i)) <= maxTolerance*mean) {
      //     break;
      //   } 
      // }
      Vector2D randomOffset = gridSampler->get_sample();
      Ray ray = camera->generate_ray((x + randomOffset.x)/width, (y + randomOffset.y)/height);
      ray.depth = max_ray_depth;
      Spectrum estIllum = est_radiance_global_illumination(ray);
      spect += estIllum;
      // s1 += estIllum.illum();
      // s2 += estIllum.illum() * estIllum.illum();
    }
    // sampleCountBuffer[x + y*width] = i;
    return spect/double(i);
  }


}

void PathTracer::raytrace_tile(int tile_x, int tile_y,
                               int tile_w, int tile_h) {

  size_t w = sampleBuffer.w;
  size_t h = sampleBuffer.h;

  size_t tile_start_x = tile_x;
  size_t tile_start_y = tile_y;

  size_t tile_end_x = std::min(tile_start_x + tile_w, w);
  size_t tile_end_y = std::min(tile_start_y + tile_h, h);

  size_t tile_idx_x = tile_x / imageTileSize;
  size_t tile_idx_y = tile_y / imageTileSize;
  size_t num_samples_tile = tile_samples[tile_idx_x + tile_idx_y * num_tiles_w];

  for (size_t y = tile_start_y; y < tile_end_y; y++) {
    if (!continueRaytracing) return;
    for (size_t x = tile_start_x; x < tile_end_x; x++) {
        Spectrum s = raytrace_pixel(x, y);
        sampleBuffer.update_pixel(s, x, y);
    }
  }

  tile_samples[tile_idx_x + tile_idx_y * num_tiles_w] += 1;
  sampleBuffer.toColor(frameBuffer, tile_start_x, tile_start_y, tile_end_x, tile_end_y);
}

void PathTracer::raytrace_cell(ImageBuffer& buffer) {
  size_t tile_start_x = cell_tl.x;
  size_t tile_start_y = cell_tl.y;

  size_t tile_end_x = cell_br.x;
  size_t tile_end_y = cell_br.y;

  size_t w = tile_end_x - tile_start_x;
  size_t h = tile_end_y - tile_start_y;
  HDRImageBuffer sb(w, h);
  buffer.resize(w,h);

  stop();
  render_cell = true;
  {
    unique_lock<std::mutex> lk(m_done);
    start_raytracing();
    cv_done.wait(lk, [this]{ return state == DONE; });
    lk.unlock();
  }

  for (size_t y = tile_start_y; y < tile_end_y; y++) {
    for (size_t x = tile_start_x; x < tile_end_x; x++) {
        buffer.data[w*(y-tile_start_y)+(x-tile_start_x)] = frameBuffer.data[x+y*sampleBuffer.w];
    }
  }
}

void PathTracer::worker_thread() {

  Timer timer;
  timer.start();

  WorkItem work;
  while (continueRaytracing && workQueue.try_get_work(&work)) {
    raytrace_tile(work.tile_x, work.tile_y, work.tile_w, work.tile_h);
    { 
      lock_guard<std::mutex> lk(m_done);
      ++tilesDone;
      cout << "\r[PathTracer] Rendering... " << int((double)tilesDone/tilesTotal * 100) << '%';
      cout.flush();
    }
  }

  workerDoneCount++;
  if (!continueRaytracing && workerDoneCount == numWorkerThreads) {
    timer.stop();
    fprintf(stdout, "\n[PathTracer] Rendering canceled!\n");
    state = READY;
  }

  if (continueRaytracing && workerDoneCount == numWorkerThreads) {
    timer.stop();
    fprintf(stdout, "\r[PathTracer] Rendering... 100%%! (%.4fs)\n", timer.duration());
    fprintf(stdout, "[PathTracer] BVH traced %llu rays.\n", bvh->total_rays);
    fprintf(stdout, "[PathTracer] Averaged %f intersection tests per ray.\n", (((double)bvh->total_isects)/bvh->total_rays));

    lock_guard<std::mutex> lk(m_done);
    state = DONE;
    cv_done.notify_one();
  }
}

void PathTracer::save_image(string filename, ImageBuffer* buffer) {

  if (state != DONE) return;

  if (!buffer)
    buffer = &frameBuffer;

  if (filename == "") {
    time_t rawtime;
    time (&rawtime);

    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    stringstream ss;
    ss << this->filename << "_screenshot_" << lt->tm_mon+1 << "-" << lt->tm_mday << "_" 
      << lt->tm_hour << "-" << lt->tm_min << "-" << lt->tm_sec << ".png";
    filename = ss.str();  
  }

  uint32_t* frame = &buffer->data[0];
  size_t w = buffer->w;
  size_t h = buffer->h;
  uint32_t* frame_out = new uint32_t[w * h];
  for(size_t i = 0; i < h; ++i) {
    memcpy(frame_out + i * w, frame + (h - i - 1) * w, 4 * w);
  }
  
  for (size_t i = 0; i < w * h; ++i) {
    frame_out[i] |= 0xFF000000;
  }

  fprintf(stderr, "[PathTracer] Saving to file: %s... ", filename.c_str());
  lodepng::encode(filename, (unsigned char*) frame_out, w, h);
  fprintf(stderr, "Done!\n");
  
  delete[] frame_out;

  save_sampling_rate_image(filename);
}

void PathTracer::save_sampling_rate_image(string filename) {
  size_t w = frameBuffer.w;
  size_t h = frameBuffer.h;
  ImageBuffer outputBuffer(w, h);

  for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
          float samplingRate = sampleCountBuffer[y * w + x] * 1.0f / ns_aa;

          Color c;
          if (samplingRate <= 0.5) {
              float r = (0.5 - samplingRate) / 0.5;
              c = Color(0.0f, 0.0f, 1.0f) * r + Color(0.0f, 1.0f, 0.0f) * (1.0 - r);
          } else {
              float r = (1.0 - samplingRate) / 0.5;
              c = Color(0.0f, 1.0f, 0.0f) * r + Color(1.0f, 0.0f, 0.0f) * (1.0 - r);
          }
          outputBuffer.update_pixel(c, x, h - 1 - y);
      }
  }
  uint32_t* frame_out = new uint32_t[w * h];
  
  for (size_t i = 0; i < w * h; ++i) {
    uint32_t out_color_hex = 0;
    frame_out[i] = outputBuffer.data.data()[i];
    frame_out[i] |= 0xFF000000;
  }

  lodepng::encode(filename.substr(0,filename.size()-4) + "_rate.png", (unsigned char*) frame_out, w, h);
  
  delete[] frame_out;
}

}  // namespace CGL
