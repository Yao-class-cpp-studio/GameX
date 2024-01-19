#include "GameBall/logic/units/regular_ball.h"

#include "GameBall/core/game_ball.h"
#include "GameBall/logic/events.h"
#include "GameBall/logic/world.h"
#include "IrrKlang/PlaySound.h"

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
  sphere.elasticity = sphere.elasticities[sphere.type];
  sphere.friction = sphere.frictions[sphere.type];
  sphere.gravity = glm::vec3{0.0f, -9.8f, 0.0f};
}

RegularBall::~RegularBall() {
  ;
}

SYNC_ACTOR_FUNC(RegularBall) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  actor->SetMass(sphere.masses[sphere.type]);
  actor->SetGravity(glm::vec3{0.0f, -9.8f, 0.0f});
  actor->SetTransform(glm::mat3{radius_});
  actor->SetMotion(position_, velocity_, orientation_, augular_momentum_);
  actor->SetMomentOfInertia(sphere.inertia[0][0]);
}

long double last_change_time = -1, cur_time = 0;
glm::vec3 player_pos = {0.0f, 0.0f, 0.0f};
void RegularBall::UpdateTick() {
  float delta_time = world_->TickDeltaT();
  cur_time += delta_time;
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  bool restart = false;
  float delta_v = 0.5f, delta_av = 0.2f;
  auto owner = world_->GetPlayer(player_id_);

  if (owner) {
    if (UnitId() == owner->PrimaryUnitId()) {
      // Controls player ball
      auto input = owner->TakePlayerInput();

      glm::vec3 forward = glm::normalize(glm::vec3{input.orientation});
      glm::vec3 right =
          glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));

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
      if (input.left_arrow && (cur_time - last_change_time >= 1.0) &&
          sphere.radius == 1.0f) {
        sphere.type = (sphere.type - 1) < 0 ? 2 : (sphere.type - 1);
        last_change_time = cur_time;
        LAND_INFO("Sphere changed into type {}.", sphere.type);
        PlaySound::Play(R"(../../../assets/audio/pop.mp3)");
      }
      if (input.right_arrow && (cur_time - last_change_time >= 1.0) &&
          sphere.radius == 1.0f) {
        sphere.type = (sphere.type + 1) % 3;
        last_change_time = cur_time;
        LAND_INFO("Sphere changed into type {}.", sphere.type);
        PlaySound::Play(R"(../../../assets/audio/pop.mp3)");
      }
      if (input.shrink && (cur_time - last_change_time >= 1.0) &&
          sphere.radius > 0.25f) {
        sphere.position =
            sphere.position + glm::vec3{0.0f, -0.5f * sphere.radius, 0.0f};
        sphere.radius *= 0.5f;
        sphere.mass *= 0.125f;
        last_change_time = cur_time;
        LAND_INFO("Sphere shrank.");
      }
      if (input.grow && (cur_time - last_change_time >= 1.0) &&
          sphere.radius < 4.0f) {
        sphere.position =
            sphere.position + glm::vec3{0.0f, sphere.radius, 0.0f};
        sphere.radius *= 2.0f;
        sphere.mass *= 8.0f;
        last_change_time = cur_time;
        LAND_INFO("Sphere enlarged.");
      }
      if (input.brake) {
        PlaySound::Play(R"(../../../assets/audio/brake.mp3)");
        sphere.angular_velocity *= 0.7f;
        input.speed_up = false;
      } else if (input.speed_up) {
        PlaySound::Play(R"(../../../assets/audio/acce.mp3)");
        sphere.velocity *= std::pow(3 * delta_v, delta_time);
        sphere.angular_velocity *= std::pow(3 * delta_av, delta_time);
      } else {
        sphere.velocity *= std::pow(delta_v, delta_time);
        sphere.angular_velocity *= std::pow(delta_av, delta_time);
      }
      restart = input.restart || sphere.position.y <= -7.0f;
      if (restart) {
        LAND_INFO("You LOSE~");
        PlaySound::Play(R"(../../../assets/audio/lose.mp3)");
        sphere.position = glm::vec3{0.0f, sphere.radius + 0.1f, 0.0f};
      }

      if (glm::length(moving_direction) > 0.0f) {
        moving_direction = glm::normalize(moving_direction);
        sphere.angular_velocity +=
            moving_direction * angular_acceleration * delta_time;
      }

      sphere.elasticity = sphere.elasticities[sphere.type];
      sphere.friction = sphere.frictions[sphere.type];
      sphere.mass = sphere.masses[sphere.type];
      player_pos = sphere.position;
    } else {
      // Controls enemy ball
      if(cur_time>=5.0){
        glm::vec3 forward = glm::normalize(player_pos - sphere.position);
        glm::vec3 back =
            glm::normalize(glm::vec3{0.0f, 0.0f, 0.0f} - sphere.position);
        glm::vec3 right =
            glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));
        back = glm::normalize(glm::cross(back, glm::vec3{0.0f, 1.0f, 0.0f}));
        glm::vec3 moving_direction{};
        if (abs(sphere.position.x) <= 38.0f && abs(sphere.position.z) <= 38.0f)
          moving_direction -= right;
        else
          moving_direction -= back;
        float angular_acceleration = glm::radians(2880.0f);

        if (glm::length(moving_direction) > 0.0f) {
          moving_direction = glm::normalize(moving_direction);
          sphere.angular_velocity +=
              moving_direction * angular_acceleration * delta_time;
        }
      }

      sphere.velocity *= std::pow(delta_v, delta_time);
      sphere.angular_velocity *= std::pow(delta_av, delta_time);
      if (sphere.position.y <= -7.0f) {
        LAND_INFO("You WON!!");
        PlaySound::Play(R"(../../../assets/audio/win.mp3)");
        sphere.position = glm::vec3{-5.0f, sphere.radius + 0.1f, 0.0f};
        restart = true;
      }
    }
  }

  if (!restart) {
    position_ = sphere.position;
    velocity_ = sphere.velocity;
    orientation_ = sphere.orientation;
    augular_momentum_ = sphere.inertia * sphere.angular_velocity;
  } else {
    velocity_ = sphere.velocity = glm::vec3{0.0f};
    augular_momentum_ = sphere.angular_velocity = glm::vec3{0.0f};
    orientation_ = sphere.orientation = glm::mat3{1.0f};
  }
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
