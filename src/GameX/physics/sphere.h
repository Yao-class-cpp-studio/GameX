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
  float elasticities[3] = {1.0f, 3.0f, 5.0f};
  float masses[3] = {1.0f, 0.6f, 0.3f};
  float frictions[3] = {10.0f, 20.0f, 30.0f};
};
}  // namespace GameX::Physics
