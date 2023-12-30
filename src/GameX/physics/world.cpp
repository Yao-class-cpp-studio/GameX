#include "GameX/physics//world.h"

#include <functional>
#include <random>
#include <vector>

#include "GameX/physics/collision.h"

namespace GameX::Physics {
World::World() {
}

World::~World() {
}
void World::Update(float delta_time) {
  for (auto &sphere : spheres_) {
    sphere.second.Update(delta_time);
  }
  for (auto &cube : cubes_) {
    cube.second.Update(delta_time);
  }
}

uint64_t World::CreateSphere(float radius, float mass) {
  uint64_t id = next_sphere_id_++;
  spheres_.emplace(id, Sphere(radius, mass));
  return id;
}

uint64_t World::CreateCube(float side_length, float mass) {
  uint64_t id = next_cube_id_++;
  cubes_.emplace(id, Cube(mass, side_length));
  return id;
}

Sphere &World::GetSphere(uint64_t id) {
  return spheres_.at(id);
}

Cube &World::GetCube(uint64_t id) {
  return cubes_.at(id);
}

void World::SolveCollisions() {
  std::vector<std::tuple<RigidBody *, RigidBody *, Collision>> collision_pairs;

  for (auto &sphere1 : spheres_) {
    for (auto &sphere2 : spheres_) {
      if (sphere1.first >= sphere2.first) {
        continue;
      }
      Collision collision;
      if (DetectCollision(sphere1.second, sphere2.second, collision)) {
        collision_pairs.emplace_back(&sphere1.second, &sphere2.second,
                                     collision);
      }
    }
    for (auto &cube : cubes_) {
      Collision collision;
      if (DetectCollision(sphere1.second, cube.second, collision)) {
        collision_pairs.emplace_back(&sphere1.second, &cube.second, collision);
      }
    }
  }

  // Random shuffle with std random library
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(collision_pairs.begin(), collision_pairs.end(), g);

  bool collision_found = false;
  do {
    collision_found = false;
    for (auto &pair : collision_pairs) {
      if (SolveCollision(*std::get<0>(pair), *std::get<1>(pair),
                         std::get<2>(pair))) {
        collision_found = true;
      }
    }
  } while (collision_found);
}

void World::ApplyGravity(float delta_time) {
  for (auto &sphere : spheres_) {
    sphere.second.velocity += sphere.second.gravity * delta_time;
  }
  for (auto &cube : cubes_) {
    cube.second.velocity += cube.second.gravity * delta_time;
  }
}
}  // namespace GameX::Physics
