#ifndef CGL_MESHEDIT_H
#define CGL_MESHEDIT_H

#include "halfEdgeMesh.h"

using namespace std;

namespace CGL {

class MeshResampler{
public:

  MeshResampler(){};
  ~MeshResampler(){}

  void upsample  ( HalfedgeMesh& mesh );
  void downsample( HalfedgeMesh& mesh );
  void resample  ( HalfedgeMesh& mesh );
};

} // namespace CGL

#endif // CGL_MESHEDIT_H
