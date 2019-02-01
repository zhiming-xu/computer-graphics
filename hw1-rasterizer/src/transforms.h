#ifndef CGL_TRANSFORMS_H
#define CGL_TRANSFORMS_H
#include "CGL/CGL.h"

namespace CGL {

Vector2D operator*(const Matrix3x3 &m, const Vector2D &v);

Matrix3x3 translate(float dx, float dy);
Matrix3x3 scale(float sx, float sy);
Matrix3x3 rotate(float deg);

}


#endif // CGL_TRANSFORMS_H