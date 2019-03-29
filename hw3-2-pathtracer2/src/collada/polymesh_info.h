#ifndef CGL_COLLADA_MESHINFO_H
#define CGL_COLLADA_MESHINFO_H

#include "CGL/vector2D.h"

#include "collada_info.h"
#include "material_info.h"

namespace CGL { namespace Collada {

struct Polygon {

  std::vector<size_t> vertex_indices;   ///< indices into vertex array
  std::vector<size_t> normal_indices;   ///< indices into normal array
  std::vector<size_t> texcoord_indices; ///< indices into texcoord array

}; // struct Polygon

typedef std::vector<Polygon> PolyList;
typedef PolyList::iterator PolyListIter;

struct PolymeshInfo : Instance {

  std::vector<Vector3D> vertices;   ///< polygon vertex array
  std::vector<Vector3D> normals;    ///< polygon normal array
  std::vector<Vector2D> texcoords;  ///< texture coordinate array

  std::vector<Polygon>  polygons;   ///< polygons

  MaterialInfo* material;  ///< material of the mesh

}; // struct Polymesh

std::ostream& operator<<(std::ostream& os, const PolymeshInfo& polymesh);

} // namespace Collada
} // namespace CGL

#endif // CGL_COLLADA_MESHINFO_H
