#ifndef BEZIERCURVE_H
#define	BEZIERCURVE_H

#include "CGL/CGL.h"
#include "CGL/renderer.h"

using namespace std;

namespace CGL {

  class BezierCurve : public Renderer {
  public:
    BezierCurve(int numControlPoints);

    void loadControlPoints(FILE* file);
    void evaluateStep();
    void drawCurve();

    // inherited Renderer interface functions
    void render();
    void init() {}
    void resize(size_t w, size_t h) {
      width = w;
      height = h;
    }
    string name() { return "Bezier Curve"; }
    string info() { return ""; }
    void key_event(char key);
    void cursor_event(float x, float y, unsigned char keys);
    void scroll_event(float offset_x, float offset_y);

  protected:
    std::vector<Vector2D> controlPoints; // The original control points of the Bezier curve

    std::vector< std::vector<Vector2D> > evaluatedLevels; // Levels of points evaluated through de Casteljau subdivision
                                                          // evaluatedLevels[0] is a vector<Vector2D> containing the original N control points
                                                          // evaluatedLevels[1] is a vector<Vector2D> containing N-1 evaluated points using de Casteljau's algorithm
                                                          // For a cubic Bezier curve, the 2D vector should look like this when fully evaluated:
                                                          // * * * *
                                                          // * * *
                                                          // * *
                                                          // *

    float t; // Value between 0 and 1 to evaluate the Bezier curve at
    int numControlPoints;

    size_t width, height;
    int selected = -1;
    int eval_level = 0;
    int drawingCurve = 0;
  };

}
#endif	/* BEZIERCURVE_H */
