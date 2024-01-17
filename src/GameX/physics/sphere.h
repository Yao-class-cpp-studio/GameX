#pragma once
#include "GameX/physics/rigid_body.h"
#include "GameX/utils/utils.h"

namespace GameX::Physics {
struct Sphere : public RigidBody {
  Sphere(float radius = 1.0f, float mass = 1.0f);
  ~Sphere();

  void SetRadiusMass(float radius = 1.0f, float mass = 1.0f);

  float radius{1.0f};
  int type = 0;
  float elasticities[3] = {1.0f, 2.0f, 3.0f};
  float masses[3] = {1.0f, 0.7f, 0.5f};
  float frictions[3] = {10.0f, 13.0f, 15.0f};
};
}  // namespace GameX::Physics
