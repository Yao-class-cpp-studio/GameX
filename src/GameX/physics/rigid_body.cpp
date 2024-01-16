#include "GameX/physics/rigid_body.h"


namespace GameX::Physics {

void RigidBody::Update(float delta_time) {
  position += velocity * delta_time;
  if (position.y < -20.0) {
    position = ini_position + glm::vec3{0.0f, 5.0f, 0.0f};
    velocity = glm::vec3{0.0f, 0.0f, 0.0f};
    angular_velocity = glm::vec3{0.0f, 0.0f, 0.0f};
    orientation = glm::mat3{1.0f};
  }
  orientation = Base::Rotate(angular_velocity * delta_time) * orientation;
}

}  // namespace GameX::Physics
