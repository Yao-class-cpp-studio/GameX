#pragma once
#include "GameX/physics/cube.h"
#include "GameX/physics/rigid_body.h"
#include "GameX/physics/sphere.h"
#include "GameX/utils/utils.h"

namespace GameX::Physics {
struct Collision {
  glm::vec3 point;
  glm::vec3 normal;
  float penetration;
};

bool DetectCollision(const Sphere &sphere1,
                     const Sphere &sphere2,
                     Collision &collision);

bool DetectCollision(const Sphere &sphere,
                     const Cube &cube,
                     Collision &collision);

uint64_t SolveCollision(RigidBody &body1,
                    RigidBody &body2,
                    const Collision &collision);

}  // namespace GameX::Physics
