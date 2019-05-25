#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  this->point_masses.reserve(this->num_height_points * this->num_width_points);
  switch(this->orientation)
  {
      case HORIZONTAL:
          for(auto i=0; i<this->num_height_points; ++i)
          {
              for(auto j=0; j<this->num_width_points; ++j)
              {
                  auto vec = Vector3D(j * width/(num_width_points - 1.), 1., i * height/(num_height_points - 1.));
                  auto is_pinned = false;
                  for(const auto vec_iter: this->pinned)
                  {
                      if(vec_iter[0]==j && vec_iter[1]==i)
                      {
                          is_pinned = true;
                          break;
                      }
                  }
                  auto pm = PointMass(vec, is_pinned);
                  this->point_masses.emplace_back(pm);
              }
          }
          break;
      case VERTICAL:
          for(auto i=0; i<this->num_height_points; ++i)
          {
              for(auto j=0; j<this->num_width_points; ++j)
              {
                  auto rand_offset = (1. * rand() / RAND_MAX - 1.) / 1000.;
                  auto vec = Vector3D(j * width/(num_width_points - 1.), i * height/(num_height_points - 1.), rand_offset);
                  auto is_pinned = false;
                  for(const auto vec_iter: this->pinned)
                  {
                      if(vec_iter[0]==j && vec_iter[1]==i)
                      {
                          is_pinned = true;
                          break;
                      }
                  }
                  auto pm = PointMass(vec, is_pinned);
                  this->point_masses.emplace_back(pm);
              }
          }
          break;
      default:
          cout<<"\033[31mShould not reach here, orientation "<<orientation<<" does not exist!\033[0m"<<endl;
  }
  this->springs.reserve(num_height_points * num_width_points * 6);
  for(auto i=0; i<num_height_points; ++i)
  {
      for(auto j=0; j<num_width_points; ++j)
      {
          if(j > 0)
          {
              auto spring = Spring(&point_masses[i*num_width_points+j], &point_masses[i*num_width_points+j-1], STRUCTURAL);
              this->springs.emplace_back(spring);
          }
          if(i > 0)
          {
              auto spring = Spring(&point_masses[i*num_width_points+j], &point_masses[(i-1)*num_width_points+j], STRUCTURAL);
              this->springs.emplace_back(spring);
              if(j > 0)
              {
                  auto spring = Spring(&point_masses[i*num_width_points+j], &point_masses[(i-1)*num_width_points+j-1], SHEARING);
                  this->springs.emplace_back(spring);
              }
              if(j < num_width_points - 1)
              {
                  auto spring = Spring(&point_masses[i*num_width_points+j], &point_masses[(i-1)*num_width_points+j+1], SHEARING);
                  this->springs.emplace_back(spring);
              }
          }
          if(j > 1)
          {
              auto spring = Spring(&point_masses[i*num_width_points+j], &point_masses[i*num_width_points+j-2], BENDING);
              this->springs.emplace_back(spring);
          }
          if(i > 1)
          {
              auto spring = Spring(&point_masses[i*num_width_points+j], &point_masses[(i-2)*num_width_points+j], BENDING);
              this->springs.emplace_back(spring);
          }
      }
  }

}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.
  Vector3D total_external_forces;
  for(auto acceleration_iter: external_accelerations)
      total_external_forces += acceleration_iter;
  total_external_forces *= mass;
  for(auto &pm: this->point_masses)
      pm.forces = total_external_forces;
  for(auto &spring: this->springs)
  {
      if((cp->enable_structural_constraints && spring.spring_type == STRUCTURAL) ||
         (cp->enable_shearing_constraints && spring.spring_type == SHEARING) ||
         (cp->enable_bending_constraints && spring.spring_type == BENDING))
      {
          auto k_s = cp->ks;
          if(spring.spring_type == BENDING)
              k_s *= .2;
          auto f = (spring.pm_b->position - spring.pm_a->position).unit() * k_s *
                   ((spring.pm_b->position - spring.pm_a->position).norm() - spring.rest_length);
          spring.pm_a->forces += f;
          spring.pm_b->forces -= f;
      }
  }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for(auto &pm: this->point_masses)
  {
      if(!pm.pinned)
      {
          auto acceleration = pm.forces / mass;
          auto last_position = pm.last_position;
          pm.last_position = pm.position;
          pm.position += (1 - cp->damping / 100.) * (pm.position - last_position) +
                          acceleration * delta_t * delta_t;
      }
  }

  // TODO (Part 4): Handle self-collisions.
  build_spatial_map();
  // TODO (Part 3): Handle collisions with other primitives.
  for(auto &pm: this->point_masses)
  {
      // part 4, check for self-collisions
      self_collide(pm, simulation_steps);
      // part 3, check collisions with other primitives
      for(auto object: *collision_objects)
      {
          object->collide(pm);
      }
  }


  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  for(auto &spring: this->springs)
  {
      auto diff = (spring.pm_a->position-spring.pm_b->position).norm() - spring.rest_length * 1.1;
      if(diff>0)
      {
          // need correction
          auto correction = (spring.pm_a->position - spring.pm_b->position).unit() * 0.5 * diff;
          if(spring.pm_a->pinned && !spring.pm_b->pinned)
          {
              spring.pm_b->position += 2 * correction;
          }
          else if(spring.pm_b->pinned && !spring.pm_a->pinned)
          {
              spring.pm_a->position -= 2 * correction;
          }
          else
          {
              spring.pm_a->position -= correction;
              spring.pm_b->position += correction;
          }
      }
  }

}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  for(auto &pm: this->point_masses)
  {
      auto hash = hash_position(pm.position);
      if(!map[hash])
      {
          map[hash] = new vector<PointMass *>();
      }
      map[hash]->push_back(&pm);
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
  auto hash = hash_position(pm.position);
  int cnt = 0;
  Vector3D correction;
  if(map[hash])
  {
      for(auto *pm_iter: *map[hash])
      {
          auto diff = pm.position - pm_iter->position;
          if(pm_iter!=&pm && diff.norm()<=2*thickness)
          {
              correction += diff.unit() * (2 * thickness - diff.norm());
              cnt++;
          }
      }
  }
  if(cnt)
      pm.position += correction / cnt / simulation_steps;
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
  auto w = 3.*width/num_width_points, h = 3.*height/num_height_points, t = max(w, h);
  auto x = floor(pos.x / w), y = floor(pos.y / h), z = floor(pos.z / t);
  return (x * 31 + y) * 31 + z;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
