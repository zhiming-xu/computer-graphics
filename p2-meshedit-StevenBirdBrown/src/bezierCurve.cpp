#include "bezierCurve.h"
#include "CGL/misc.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>

using namespace std;

namespace CGL {

  BezierCurve::BezierCurve(int numControlPoints)
  {
    this->numControlPoints = numControlPoints;
    t = 0.5;
    controlPoints = std::vector<Vector2D>(numControlPoints);
  }

  void BezierCurve::loadControlPoints(FILE* file)
  {
    for (int i = 0; i < numControlPoints; i++)
    {
      fscanf(file, "%lf %lf", &controlPoints[i].x, &controlPoints[i].y);
    }

    evaluatedLevels.push_back(std::vector<Vector2D>(controlPoints));
  }

  void BezierCurve::render()
  {
    // Set up color and size
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(10.0f);

    glClearColor(0, 0, 0, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect =  height/(float)width;
    if (aspect < 1)
      glOrtho(0, 1/aspect, 0, 1, 1, 0);
    else
      glOrtho(0, 1, 0, aspect, 1, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPointSize(20.0f);
    // Draw control points
    glBegin(GL_POINTS);
    for (int i = 0; i < controlPoints.size(); i++)
    {
      if (selected == i)
        glColor3f(0.0, 1.0, 0.0);
      else
        glColor3f(1.0, 1.0, 1.0);
      Vector2D pt = controlPoints[i];
      glVertex2f(pt.x, pt.y);
    }
    glEnd();

    glLineWidth(2.);
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    for (int i = 0; i < controlPoints.size() - 1; i++)
    {
      Vector2D pt1 = controlPoints[i];
      Vector2D pt2 = controlPoints[i+1];
      glVertex2f(pt1.x, pt1.y);
      glVertex2f(pt2.x, pt2.y);
    }
    glEnd();

    glFlush();

    if (drawingCurve)
    {
      drawCurve();
    }

    if (eval_level) {
      evaluatedLevels.clear();
      evaluatedLevels.push_back(controlPoints);

      for (int i = 0; i < eval_level; ++i)
        evaluateStep();

      glColor3f(0.0, 0.0, 1.0);
      glBegin(GL_LINES);
      for (int level = 1; level < evaluatedLevels.size(); level++)
      {
        std::vector<Vector2D>& evaluatedPoints = evaluatedLevels[level];
        for (int i = 0; i < evaluatedPoints.size() - 1; i++)
        {
          Vector2D& pt1 = evaluatedPoints[i];
          Vector2D& pt2 = evaluatedPoints[i+1];
          glVertex2f(pt1.x, pt1.y);
          glVertex2f(pt2.x, pt2.y);
        }
      }
      glEnd();


      glColor3f(0.0, 0.0, 1.0);
      glBegin(GL_POINTS);
      for (int level = 1; level < evaluatedLevels.size(); level++)
      {
        std::vector<Vector2D>& evaluatedPoints = evaluatedLevels[level];
        for (int i = 0; i < evaluatedPoints.size(); i++)
        {
          Vector2D& pt = evaluatedPoints[i];
          if (level == numControlPoints - 1)
            glColor3f(1.0, 0.0, 0.0);
          glVertex2f(pt.x, pt.y);
        }
      }
      glEnd();
    }

    glFlush();
  }

  void BezierCurve::drawCurve()
  {
    std::vector<Vector2D> curvePoints;

    float old_t = t;

    for (float p = 0.0; p <= 1.0f; p += 0.005f)
    {
      t = p;
      evaluatedLevels.clear();
      evaluatedLevels.push_back(controlPoints);
      for (int i = 0; i < numControlPoints - 1; i++)
      {
        evaluateStep();
      }
      std::vector<Vector2D>& lastLevel = evaluatedLevels[evaluatedLevels.size() - 1];
      curvePoints.push_back(lastLevel[0]);
    }

    glColor3f(0.0, 1.0, 0.0);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < curvePoints.size(); i++)
    {
      Vector2D& pt = curvePoints[i];
      glVertex2f(pt.x, pt.y);
    }
    glEnd();

    t = old_t;
  }

  void BezierCurve::key_event(char key)
  {
    switch (key)
    {
      case 'E':
      case 'e':
        eval_level = (eval_level + 1) % numControlPoints;
        break;

      case 'C':
      case 'c':
        drawingCurve = 1 - drawingCurve;
        break;
    }
  }

  void BezierCurve::cursor_event(float x, float y, unsigned char keys)
  {
    static float xp, yp;
    x = x / width;
    y = 1. - y / height;
    if (width >= height)
    {
      x *= width / (float)height;
    }
    else
    {
      y *= height / (float)width;
    }

    if (keys == 4)
    {
      if (selected >= 0)
      {
        float dx = x - xp;
        float dy = y - yp;
        controlPoints[selected] += Vector2D(dx, dy);
      }
      else
      {
        Vector2D xy(x,y);
        const float thresh = .05f;
        for (int i = 0; i < controlPoints.size(); i++)
        {
          if ((controlPoints[i] - xy).norm() < thresh)
          {
            selected = i;
          }
        }
      }
      xp = x;
      yp = y;

    }
    else
    {
      selected = -1;
    }
  }

  void BezierCurve::scroll_event(float offset_x, float offset_y) {
    const float scale = .01;
    if (eval_level)
    {
      t += offset_y * scale;
      if (t > 1)
        t = 1;
      if (t < 0)
        t = 0;
    }
  }
}
