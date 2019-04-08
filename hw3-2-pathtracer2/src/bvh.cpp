#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c, alpha);
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c, alpha);
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox centroid_box, bbox;

  for (Primitive *p : prims)
  {
      BBox bb = p->get_bbox();
      bbox.expand(bb);
      Vector3D c = bb.centroid();
      centroid_box.expand(c);
  }
  auto *node = new BVHNode(bbox);

  node->prims = new vector<Primitive *>(prims);
  // return node;
  if (prims.size() > max_leaf_size) {
    double x = centroid_box.extent.x;
    double y = centroid_box.extent.y;
    double z = centroid_box.extent.z;
    vector<Primitive*> l;
    vector<Primitive*> r;
    if (x > y && x > z)
    {
      double mid = centroid_box.min.x + x * 0.5;
      for (Primitive *p : prims)
      {
        if (p->get_bbox().centroid().x <= mid)
        {
          l.push_back(p);
        }
        else
        {
          r.push_back(p);
        }
      }
    }
    else if (y > z && y > x)
    {
      double mid = centroid_box.min.y + y * 0.5;
      for (Primitive *p : prims)
      {
        if (p->get_bbox().centroid().y <= mid)
        {
          l.push_back(p);
        }
        else
        {
          r.push_back(p);
        }
      }
    }
    else
    {
      double mid = centroid_box.min.z + z * 0.5;
      for (Primitive *p : prims)
      {
        if (p->get_bbox().centroid().z <= mid)
        {
          l.push_back(p);
        }
        else
        {
          r.push_back(p);
        }
      }
    }
    node->l = construct_bvh(l, max_leaf_size);
    node->r = construct_bvh(r, max_leaf_size);
  }
  return node;
}


bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {

  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.
  double t1, t2;
  if (node->bb.intersect(ray, t1, t2))
  {
    if (node->isLeaf())
    {
      for (Primitive *p : *(node->prims))
      {
        total_isects++;
        if (p->intersect(ray))
          return true;
      }
    }
    else
    {
      total_isects++;
      return intersect(ray, node->l) || intersect(ray, node->r);
    }
  }
  return false;
}

bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {

  // TODO (Part 2.3):
  // Fill in the intersect function.
  double t1, t2;
  if (node->bb.intersect(ray, t1, t2))
  {
    if (node->isLeaf())
    {
      bool intersected = false;
      for (Primitive *p : *(node->prims))
      {
        total_isects++;
        intersected = p->intersect(ray, i) || intersected;
      }
      return intersected;
    }
    else
    {
      total_isects++;
      bool inter = intersect(ray, i, node->l);
      return intersect(ray, i, node->r) || inter;
    }
  }
  return false;
}

}  // namespace StaticScene
}  // namespace CGL
