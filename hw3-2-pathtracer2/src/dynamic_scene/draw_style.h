#ifndef CGL_DYNAMICSCENE_DRAWSTYLE_H
#define CGL_DYNAMICSCENE_DRAWSTYLE_H

#include "scene.h"

namespace CGL { namespace DynamicScene {

/**
 * Used in rendering to specify how to draw the faces/meshes/lines/etc.
 */
class DrawStyle {
 public:

  void style_reset() const {
    glLineWidth(1);
    glPointSize(1);
  }

  void style_face() const {
    GLfloat temp[4];
    temp[0] = faceColor.r;
    temp[1] = faceColor.g;
    temp[2] = faceColor.b;
    temp[3] = 1.f;
    glColor4fv(temp);
  }

  void style_edge() const {
    GLfloat temp[4];
    temp[0] = edgeColor.r;
    temp[1] = edgeColor.g;
    temp[2] = edgeColor.b;
    temp[3] = 1.f;
    glColor4fv(temp);
    glLineWidth(strokeWidth);
  }

  void style_halfedge() const {
    GLfloat temp[4];
    temp[0] = halfedgeColor.r;
    temp[1] = halfedgeColor.g;
    temp[2] = halfedgeColor.b;
    temp[3] = 1.f;
    glColor4fv(temp);
    glLineWidth(strokeWidth);
  }

  void style_vertex() const {
    GLfloat temp[4];
    temp[0] = vertexColor.r;
    temp[1] = vertexColor.g;
    temp[2] = vertexColor.b;
    temp[3] = 1.f;
    glColor4fv(temp);
    glPointSize(vertexRadius);
  }

  Color halfedgeColor;
  Color vertexColor;
  Color edgeColor;
  Color faceColor;

  float strokeWidth;
  float vertexRadius;
};

} // namespace DynamicScene
} // namespace CGL

#endif //CGL_DYNAMICSCENE_DRAWSTYLE_H
