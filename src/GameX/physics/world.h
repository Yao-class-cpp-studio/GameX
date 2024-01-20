#pragma once
#include "GameX/physics/cube.h"
#include "GameX/physics/sphere.h"
#include "GameX/utils/utils.h"

namespace GameX::Physics {
class World {
 public:
  World();
  ~World();

  void Update(float delta_time);

  uint64_t CreateSphere(float radius = 1.0f, float mass = 1.0f);

  uint64_t CreateCube(float side_length = 1.0f, float mass = 1.0f);

  Sphere &GetSphere(uint64_t id);

  Cube &GetCube(uint64_t id);

  void SolveCollisions();

  void ApplyGravity(float delta_time);

 private:
  std::map<uint64_t, Sphere> spheres_;
  uint64_t next_sphere_id_{1};
  std::map<uint64_t, Cube> cubes_;
  uint64_t next_cube_id_{1};
};
}  // namespace GameX::Physics
