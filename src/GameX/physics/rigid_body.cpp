#include "GameX/physics/rigid_body.h"

namespace GameX::Physics {

void RigidBody::Update(float delta_time) {
  position += velocity * delta_time;
  orientation = Base::Rotate(angular_velocity * delta_time) * orientation;
}

}  // namespace GameX::Physics
