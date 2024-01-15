#include "GameX/physics/rigid_body.h"
#include <iostream>

namespace GameX::Physics {

void RigidBody::Update(float delta_time) {
  position += velocity * delta_time;
  if (position.y < -20.0) {
    std::cout << "GAME OVER" << std::endl;
    exit(1);
  }
  orientation = Base::Rotate(angular_velocity * delta_time) * orientation;
}

}  // namespace GameX::Physics
