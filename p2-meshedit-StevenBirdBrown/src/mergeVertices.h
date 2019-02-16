/*
* File:   vertexMerge.h
* Author: swl
*
* Created on February 5, 2016, 11:58 AM
*/

#ifndef VERTEXMERGE_H
#define	VERTEXMERGE_H

#include <unordered_map>
#include <unordered_set>

namespace CGL {

  struct VertexKey
  {
    size_t coord[3];
    size_t dim[2];

    bool operator==(const VertexKey& k) const
    {
      return k.coord[0] == coord[0] &&
      k.coord[1] == coord[1] &&
      k.coord[2] == coord[2];
    }
  };


  struct VertexHasher
  {
    std::size_t operator()(const VertexKey& k) const
    {
      return size_t(k.coord[0]) +
      size_t(k.coord[1])*k.dim[0] +
      size_t(k.coord[2])*k.dim[0]*k.dim[1];
    }
  };

  struct EdgeKey
  {
    size_t indices[2];

    EdgeKey(size_t i1, size_t i2)
    {
      indices[0] = i1 < i2 ? i1 : i2;
      indices[1] = i1 > i2 ? i1 : i2;
    }

    bool operator==(const EdgeKey& k) const
    {
      return k.indices[0] == indices[0] && k.indices[1] == indices[1];
    }
  };


  struct EdgeHasher
  {
    std::size_t operator()(const EdgeKey& k) const
    {
      return k.indices[1]*15485863 + k.indices[0];
    }
  };

  void mergeVertices(Polymesh* mesh)
  {
    double thres = 1e-10;
    size_t gridSize = 1e5;

    if(mesh->vertices.size() < 1)
    return;

    Vector3D lb, ub;
    lb = ub = mesh->vertices[0];

    std::vector<std::unordered_set<EdgeKey, EdgeHasher> > index2edges(mesh->vertices.size());

    for(unsigned i=0; i<mesh->polygons.size(); i++)
    {
      Polygon& poly = mesh->polygons[i];
      for(unsigned k=0; k<poly.vertex_indices.size(); k++)
      {
        unsigned i1 = poly.vertex_indices[k];
        unsigned i2 = poly.vertex_indices[(k+1)%poly.vertex_indices.size()];
        auto it1 = index2edges[i1].find(EdgeKey(i1, i2));
        auto it2 = index2edges[i2].find(EdgeKey(i1, i2));
        if(it1 == index2edges[i1].end())
        {
          index2edges[i1].insert(EdgeKey(i1, i2));
        }
        else
        {
          index2edges[i1].erase(it1);
        }
        if(it2 == index2edges[i2].end())
        {
          index2edges[i2].insert(EdgeKey(i1, i2));
        }
        else
        {
          index2edges[i2].erase(it2);
        }
      }
    }

    for(unsigned i = 0; i < mesh->vertices.size(); i++)
    {
      const Vector3D& v = mesh->vertices[i];
      for(int k=0; k<3; k++)
      {
        lb[k] = lb[k] < v[k] ? lb[k] : v[k];
        ub[k] = ub[k] > v[k] ? ub[k] : v[k];
      }
    }
    Vector3D dim = ub - lb;

    double max_d = std::max(dim[0], dim[1]);
    max_d = std::max(max_d, dim[2]);

    double delta = max_d / gridSize;

    std::unordered_map<VertexKey, std::vector<std::vector<unsigned> >, VertexHasher> key2indexGroups;

    for(unsigned i = 0; i < mesh->vertices.size(); i++)
    {

      const Vector3D& v = mesh->vertices[i];
      size_t base_coord[3];
      VertexKey key;
      base_coord[0] = (v[0] - lb[0]) / delta;
      base_coord[1] = (v[1] - lb[1]) / delta;
      base_coord[2] = (v[2] - lb[2]) / delta;
      key.dim[0] = dim[0] / delta;
      key.dim[1] = dim[1] / delta;
      bool merge = false;

      for(int dx = -1; dx <= 1; dx++)
      {
        for(int dy = -1; dy <= 1; dy++)
        {
          for(int dz = -1; dz <= 1; dz++)
          {
            key.coord[0] = base_coord[0] + dx;
            key.coord[1] = base_coord[1] + dy;
            key.coord[2] = base_coord[2] + dz;
            auto it = key2indexGroups.find(key);
            if(it == key2indexGroups.end())
            continue;

            for(unsigned k=0; k < it->second.size(); k++)
            {
              Vector3D& vc = mesh->vertices[it->second[k].front()];

              if(index2edges[it->second[k].front()].size() == 0 ||
              index2edges[i].size() == 0)
              {
                continue;
              }

              double dist = (v - vc).norm2();
              if(dist <= thres*max_d)
              {
                it->second[k].push_back(i);
                merge = true;
                break;
              }
            }
            if(merge)
            break;
          }
          if(merge)
          break;
        }
        if(merge)
        break;
      }
      if(!merge)
      {
        key.coord[0] = base_coord[0];
        key.coord[1] = base_coord[1];
        key.coord[2] = base_coord[2];
        std::vector<unsigned> indices(1, i);
        key2indexGroups[key].push_back(indices);
      }
    }

    std::vector<Vector3D> vertices = mesh->vertices;

    std::vector<unsigned> index_map(mesh->vertices.size());
    mesh->vertices.clear();
    for(auto it = key2indexGroups.begin(); it != key2indexGroups.end(); it++)
    {
      for(unsigned i=0; i<it->second.size(); i++)
      {
        for(unsigned k=0; k<it->second[i].size(); k++)
        {
          index_map[it->second[i][k]] = mesh->vertices.size();
        }
        mesh->vertices.push_back(vertices[it->second[i].front()]);
      }
    }

    for(unsigned i=0; i<mesh->polygons.size(); i++)
    {
      Polygon& poly = mesh->polygons[i];
      for(unsigned k=0; k<poly.vertex_indices.size(); k++)
      poly.vertex_indices[k] = index_map[poly.vertex_indices[k]];
    }

    PolyList list = mesh->polygons;
    mesh->polygons.clear();

    for(unsigned i=0; i<list.size(); i++)
    {
      Polygon& poly = list[i];
      std::unordered_set<unsigned> visited;
      bool valid = true;
      for(unsigned k=0; k<poly.vertex_indices.size(); k++)
      {
        if(visited.find(poly.vertex_indices[k]) != visited.end())
        {
          valid = false;
          break;
        }
        visited.insert(poly.vertex_indices[k]);
      }
      if(valid)
      mesh->polygons.push_back(poly);
    }
  }

}

#endif	/* VERTEXMERGE_H */
