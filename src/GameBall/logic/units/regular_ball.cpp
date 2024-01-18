#include "GameBall/logic/units/regular_ball.h"

#include "GameBall/core/game_ball.h"
#include "GameBall/logic/world.h"
#include "GameBall/core/packet_tool.h"

extern uint8_t app_type;

namespace GameBall{
  std::map<uint64_t, std::queue<std::string>> video_stream_map;
  std::map<uint64_t, uint64_t> room_id_to_client_id;
}

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

  if (app_type != 1){
    // Room / Local Update Scheme
    if (position_.y < -10.0f) {
      position_ =
          glm::vec3{0.0f, 10.0f, 0.0f};
      sphere.position = position_;
      velocity_ = glm::vec3{0.0f};
      sphere.velocity = velocity_;
      augular_momentum_ = glm::vec3{0.0f};
      sphere.angular_velocity = sphere.inertia_inv * augular_momentum_;
    }

    auto owner = world_->GetPlayer(player_id_);
    if (owner) {
      if (UnitId() == owner->PrimaryUnitId()) {
        auto input = owner->PlayerId() == 1 ? owner->TakePlayerInput() : owner->GetPlayerInput();

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

        if (input.jump) {
          if(sphere.position.y < 1.01f && sphere.position.y > 0.99f)
            sphere.velocity.y += 6.0f;  // Adjust the force as needed
        }

        // Only update angular velocity when the ball is grounded
        if (sphere.position.y < 1.01f && sphere.position.y > 0.99f) {
          if (glm::length(moving_direction) > 0.0f) {
            moving_direction = glm::normalize(moving_direction);
            sphere.angular_velocity +=
                moving_direction * angular_acceleration * delta_time;
          }

          if (input.brake) {
            sphere.angular_velocity = glm::vec3{0.0f};
          }
        }
      }
    }
  }else{

    // Client Update Scheme
    auto owner = world_->GetPlayer(player_id_);
    if (!video_stream_map.at(player_id_).empty()){
      auto frame = video_stream_map.at(player_id_).front();
      video_stream_map.at(player_id_).pop();

      float frame_data[18];
      decode_state(frame, frame_data);

      sphere.position.x = frame_data[0];
      sphere.position.y = frame_data[1];
      sphere.position.z = frame_data[2];
      sphere.velocity.x = frame_data[3];
      sphere.velocity.y = frame_data[4];
      sphere.velocity.z = frame_data[5];
      augular_momentum_.x = (glm::f32)frame_data[6];
      augular_momentum_.y = (glm::f32)frame_data[7];
      augular_momentum_.z = (glm::f32)frame_data[8];
      sphere.orientation[0][0] = (glm::f32)frame_data[9];
      sphere.orientation[0][1] = (glm::f32)frame_data[10];
      sphere.orientation[0][2] = (glm::f32)frame_data[11];
      sphere.orientation[1][0] = (glm::f32)frame_data[12];
      sphere.orientation[1][1] = (glm::f32)frame_data[13];
      sphere.orientation[1][2] = (glm::f32)frame_data[14];
      sphere.orientation[2][0] = (glm::f32)frame_data[15];
      sphere.orientation[2][1] = (glm::f32)frame_data[16];
      sphere.orientation[2][2] = (glm::f32)frame_data[17];
    }

  }


  sphere.velocity *= std::pow(0.5f, delta_time);
  // Only update angular velocity when the ball is grounded
  if (sphere.position.y < 1.01f && sphere.position.y > 0.99f) {
    sphere.angular_velocity *= std::pow(0.2f, delta_time);
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
