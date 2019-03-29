#ifndef CGL_RANDOMUTIL_H
#define CGL_RANDOMUTIL_H

#include <cstdlib>

namespace CGL {

/**
 * Returns a number distributed uniformly over [0, 1].
 */
inline double random_uniform() {
  return ((double)std::rand()) / RAND_MAX;
}

/**
 * Returns true with probability p and false with probability 1 - p.
 */
inline bool coin_flip(double p) {
  return random_uniform() < p;
}

} // namespace CGL

#endif  // CGL_RANDOMUTIL_H
