#pragma once
#include "GameX/utils/utils.h"

namespace GameX::Physics {
struct RigidBody {
  void Update(float delta_time);

  float mass{1.0f};
  glm::mat3 inertia{1.0f};      // inertia tensor without orientation
  glm::mat3 inertia_inv{1.0f};  // inverse inertia tensor without orientation
  glm::vec3 position{0.0f};
  glm::vec3 velocity{0.0f};
  glm::vec3 angular_velocity{0.0f};
  glm::mat3 orientation{1.0f};

  glm::vec3 gravity{0.0f, -9.8f, 0.0f};
  float friction{0.0f};
  float elasticity{0.0f};

  float flip_gravity_lapsed{0.0f};
};
}  // namespace GameX::Physics
