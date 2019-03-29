#include "CGL/CGL.h"
#include "CGL/viewer.h"

#define TINYEXR_IMPLEMENTATION
#include "CGL/tinyexr.h"

#include "application.h"
typedef uint32_t gid_t;
#include "image.h"
typedef uint32_t gid_t;

#include <iostream>
#include <string>

#ifdef _WIN32
#include "misc/getopt.h"
#else
#include <unistd.h>
#endif

using namespace std;
using namespace CGL;

#define msg(s) cerr << "[PathTracer] " << s << endl;

void usage(const char* binaryName) {
  printf("Usage: %s [options] <scenefile>\n", binaryName);
  printf("Program Options:\n");
  printf("  -s  <INT>        Number of camera rays per pixel\n");
  printf("  -l  <INT>        Number of samples per area light\n");
  printf("  -t  <INT>        Number of render threads\n");
  printf("  -m  <INT>        Maximum ray depth\n");
  printf("  -e  <PATH>       Path to environment map\n");
  printf("  -f  <FILENAME>   Image (.png) file to save output to in windowless mode\n");
  printf("  -r  <INT> <INT>  Width and height of output image (if windowless)\n");
  printf("  -h               Print this help message\n");
  printf("\n");
}

HDRImageBuffer* load_exr(const char* file_path) {
  
  const char* err;
  
  EXRImage exr;
  InitEXRImage(&exr);

  int ret = ParseMultiChannelEXRHeaderFromFile(&exr, file_path, &err);
  if (ret != 0) {
    msg("Error parsing OpenEXR file: " << err);
    return NULL;
  }

  for (int i = 0; i < exr.num_channels; i++) {
    if (exr.pixel_types[i] == TINYEXR_PIXELTYPE_HALF) {
      exr.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
    }
  }

  ret = LoadMultiChannelEXRFromFile(&exr, file_path, &err);
  if (ret != 0) {
    msg("Error loading OpenEXR file: " << err);
    exit(EXIT_FAILURE);
  }

  HDRImageBuffer* envmap = new HDRImageBuffer();
  envmap->resize(exr.width, exr.height);
  float* channel_r = (float*) exr.images[2];
  float* channel_g = (float*) exr.images[1];
  float* channel_b = (float*) exr.images[0];
  for (size_t i = 0; i < exr.width * exr.height; i++) {
    envmap->data[i] = Spectrum(channel_r[i], 
                               channel_g[i], 
                               channel_b[i]);
  }

  return envmap;
}

int main( int argc, char** argv ) {

  // get the options
  AppConfig config; int opt;
  bool write_to_file = false;
  size_t w = 0, h = 0, x = -1, y = 0, dx = 0, dy = 0;
  string filename, cam_settings = "";
  while ( (opt = getopt(argc, argv, "s:l:t:m:e:h:H:f:r:c:a:p:b:d:")) != -1 ) {  // for each option...
    switch ( opt ) {
      case 'f':
          write_to_file = true;
          filename  = string(optarg);
          break;
      case 'r':
          w = atoi(argv[optind-1]);
          h = atoi(argv[optind]);
          optind++;
          break;
      case 'p':
          x = atoi(argv[optind-1]);
          y = atoi(argv[optind-0]);
          dx = atoi(argv[optind+1]);
          dy = atoi(argv[optind+2]);
          optind += 3;
          break;
      case 's':
          config.pathtracer_ns_aa = atoi(optarg);
          break;
      case 'l':
          config.pathtracer_ns_area_light = atoi(optarg);
          break;
      case 't':
          config.pathtracer_num_threads = atoi(optarg);
          break;
      case 'm':
          config.pathtracer_max_ray_depth = atoi(optarg);
          break;
      case 'b':
          config.pathtracer_lensRadius = atof(optarg);
          break;
      case 'd':
          config.pathtracer_focalDistance = atof(optarg);
          break;
      case 'e':
          config.pathtracer_envmap = load_exr(optarg);
          break;
      case 'c':
          cam_settings = string(optarg);
          break;
      case 'a':
          config.pathtracer_samples_per_patch = atoi(argv[optind-1]);
          config.pathtracer_max_tolerance = atof(argv[optind]);
          optind++;
          break;
      case 'H':
          config.pathtracer_direct_hemisphere_sample = true;
          optind--;
          break;
      default:
          usage(argv[0]);
          return 1;
      }
  }

  // print usage if no argument given
  if (optind >= argc) {
    usage(argv[0]);
    return 1;
  }

  string sceneFilePath = argv[optind];
  msg("Input scene file: " << sceneFilePath);
  string sceneFile = sceneFilePath.substr(sceneFilePath.find_last_of('/')+1);
  sceneFile = sceneFile.substr(0,sceneFile.find(".dae"));
  config.pathtracer_filename = sceneFile;

  // parse scene
  Collada::SceneInfo *sceneInfo = new Collada::SceneInfo();
  if (Collada::ColladaParser::load(sceneFilePath.c_str(), sceneInfo) < 0) {
    delete sceneInfo;
    exit(0);
  }

  // create application
  Application *app  = new Application(config, !write_to_file);

  // write straight to file without opening a window if -f option provided
  if (write_to_file) {
    app->init();
    app->load(sceneInfo);
    delete sceneInfo;

    if (w && h)
      app->resize(w, h);
    
    if (cam_settings != "")
      app->load_camera(cam_settings);

    app->render_to_file(filename, x, y, dx, dy);
    return 0;
  }

  // create viewer
  Viewer viewer = Viewer();

  // set renderer
  viewer.set_renderer(app);

  // init viewer
  viewer.init();

  // load scene 
  app->load(sceneInfo);

  delete sceneInfo;

  if (w && h)
    viewer.resize(w, h);

  if (cam_settings != "")
    app->load_camera(cam_settings);

  // start viewer
  viewer.start();

  return 0;

}
