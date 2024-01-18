#include "GameBall/logic/units/regular_ball.h"

#include "GameBall/core/game_ball.h"
//#include "GameBall/logic/units/settlement.h"
#include "GameBall/logic/world.h"

namespace GameBall::Logic::Units {
RegularBall::RegularBall(World *world,
                         uint64_t player_id,
                         const glm::vec3 &position,
                         float radius,
                         float mass)
    : Unit(world, player_id) {
  radius_ = radius;
  mass_ = mass;
  position_ = position;
  auto physics_world = world_->PhysicsWorld();
  sphere_id_ = physics_world->CreateSphere();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.position = position_;
  sphere.SetRadiusMass(radius_, mass_);
  sphere.orientation = orientation_;
  sphere.velocity = velocity_;
  sphere.angular_velocity = glm::vec3{0.0f};
  sphere.elasticity = 1.0f;
  sphere.friction = 10.0f;
  sphere.gravity = glm::vec3{0.0f, -9.8f, 0.0f};
}

RegularBall::~RegularBall() {
  ;
}

SYNC_ACTOR_FUNC(RegularBall) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  actor->SetMass(1.0f);
  actor->SetGravity(glm::vec3{0.0f, -9.8f, 0.0f});
  actor->SetTransform(glm::mat3{radius_});
  actor->SetMotion(position_, velocity_, orientation_, augular_momentum_);
  actor->SetMomentOfInertia(sphere.inertia[0][0]);
}

void RegularBall::UpdateTick() {
  float delta_time = world_->TickDeltaT();
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);

  auto owner = world_->GetPlayer(player_id_);
  sphere.velocity *= std::pow(0.5f, delta_time);
  sphere.angular_velocity *= std::pow(0.2f, delta_time);

  if (owner) {
    auto input = owner->TakePlayerInput();
    if (input.original_mode) {
      std::cout << "Switch to original mode\n";
      world_->difficulty = 0;
    }
    if (input.easy_mode) {
      std::cout << "Switch to easy mode\n";
      world_->difficulty = 2;
    }
    if (input.medium_mode) {
      std::cout << "Switch to medium mode\n";
      world_->difficulty = 4;
    }
    if (input.difficult_mode) {
      std::cout << "Switch to difficult mode\n";
      world_->difficulty = 6;
    }
    if (UnitId() == owner->PrimaryUnitId()) {
      glm::vec3 forward = glm::normalize(glm::vec3{input.orientation});
      glm::vec3 right =
          glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));
      glm::vec3 z_rotate_speed =
          glm::normalize(glm::cross(forward, glm::vec3{0.0f, 0.0f, 1.0f}));

      glm::vec3 moving_direction{};

      float angular_acceleration = glm::radians(2880.0f);

      if (input.move_forward) {
        moving_direction -= right;
      }
      if (input.move_backward) {
        moving_direction += right;
      }
      if (input.move_left) {
        moving_direction -= forward;
      }
      if (input.move_right) {
        moving_direction += forward;
      }
      if (input.rotate_z) {
        moving_direction += z_rotate_speed;
      }
      if (input.move_up) {
        if (jump == 1 && sphere.velocity.y < 0) {
          sphere.velocity.y = 5;
          jump++;
          // std::cout << "double jump" << std::endl;
        } else if (jump == 0) {
          sphere.velocity.y += 3.5;
          jump++;
          // std::cout << "first jump" << std::endl;
        }
        // std::cout << sphere.velocity.y << " " << sphere.position.y <<
        // std::endl;
      }
      if (glm::length(moving_direction) > 0.0f) {
        moving_direction = glm::normalize(moving_direction);
        sphere.angular_velocity +=
            moving_direction * angular_acceleration * delta_time;
      }

      if (input.brake) {
        sphere.angular_velocity = glm::vec3{0.0f};
      }
      world_->owner_place = sphere.position;
    }
    else {
      glm::vec3 relative_pos = glm::normalize(sphere.position - world_->owner_place);
      sphere.velocity -= 0.01f * world_->difficulty * relative_pos;
    }
  }

  position_ = sphere.position;
  if (position_.y < -30) {
    std::string result;
    if (owner->is_enemy) {
      result = "you win";
      sphere.position = glm::vec3{-5.0f, 1.0f, 0.0f};
      world_->restart = 1;
      world_->score[0]++;
    } else {
      result = "you lose";
      sphere.position = glm::vec3{0.0f, 1.0f, 0.0f};
      world_->restart = 2;
      world_->score[1]++;
    }
    result += "\t\tSCORE: " + std::to_string(world_->score[0]) + " : " +
              std::to_string(world_->score[1]);
    showWindow(result);
    // std::cout << "SCORE: " << world_->score[0] << " " << world_->score[1]
    //           << std::endl;
  } else if (position_.y > 0 && position_.y < 1) {
    jump = 0;
  }
  if (world_->restart == 1 && !owner->is_enemy) {
    sphere.position = glm::vec3{0.0f, 1.0f, 0.0f};
    world_->restart = 0;
  } else if (world_->restart == 2 && owner->is_enemy) {
    sphere.position = glm::vec3{-5.0f, 1.0f, 0.0f};
    world_->restart = 0;
  }
  position_ = sphere.position;
  velocity_ = sphere.velocity;
  orientation_ = sphere.orientation;
  augular_momentum_ = sphere.inertia * sphere.angular_velocity;
}

void RegularBall::SetMass(float mass) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius_, mass);
  mass_ = mass;
}

void RegularBall::SetGravity(const glm::vec3 &gravity) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.gravity = gravity;
}

void RegularBall::SetRadius(float radius) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius, mass_);
  radius_ = radius;
}

void RegularBall::SetMotion(const glm::vec3 &position,
                            const glm::vec3 &velocity,
                            const glm::mat3 &orientation,
                            const glm::vec3 &angular_momentum) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.position = position;
  sphere.velocity = velocity;
  sphere.orientation = orientation;
  sphere.angular_velocity = sphere.inertia_inv * angular_momentum;
  position_ = position;
  velocity_ = velocity;
  orientation_ = orientation;
  augular_momentum_ = angular_momentum;
}

glm::vec3 RegularBall::Position() const {
  return position_;
}

glm::vec3 RegularBall::Velocity() const {
  return velocity_;
}

glm::mat3 RegularBall::Orientation() const {
  return orientation_;
}

glm::vec3 RegularBall::AngularMomentum() const {
  return augular_momentum_;
}

}  // namespace GameBall::Logic::Units
