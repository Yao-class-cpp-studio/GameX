#include "GameBall/logic/units/big_ball.h"

namespace GameBall::Logic::Units {
BigBall::BigBall(World *world,
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

BigBall::~BigBall() {
  ;
}

SYNC_ACTOR_FUNC(BigBall) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  actor->SetMass(1.0f);
  actor->SetGravity(glm::vec3{0.0f, -9.8f, 0.0f});
  actor->SetTransform(glm::mat3{radius_});
  actor->SetMotion(position_, velocity_, orientation_, augular_momentum_);
  actor->SetMomentOfInertia(sphere.inertia[0][0]);
}

void BigBall::ChangeSize(Sphere& sphere,float rad, float mas) {
  float change = rad-radius_;
  float mulr = rad / radius_;
  float mulm = mas / mass_;
  radius_ = rad;
  mass_ = mas;
  sphere.radius = radius_;
  sphere.mass = mass_;
  sphere.inertia *= (mulr*mulr*mulm);
  sphere.inertia_inv *= (mulr * mulr * mulm);
  sphere.position += glm::vec3{0.0f, change, 0.0f};
}
void BigBall::ChangeSize(Sphere &sphere,float mult) {
  ChangeSize(sphere, radius_ * mult, mass_ * pow(mult, 3));
}

void BigBall::Split(Sphere& sphere,float mass,float rad) {
  auto chooseid = world_->GetPlayer(enemy_id_)->PrimaryUnitId();
  auto obj = world_->GetUnit(chooseid);
  auto dir = dynamic_cast<Logic::Units::RegularBall *>(obj)->Position();
  glm::vec3 mv_dir = glm::normalize(dir - position_);
  glm::vec3 npos = position_+mv_dir*radius_/2.0f;
   auto bullet = world_->CreateUnit<Logic::Units::Bullet>(
      player_id_, npos,
       rad,mass);
  bullet->SetMotion(bullet->Position(),
                     mv_dir * 5.0f,
                    orientation_, glm::vec3{0.0f});
   sphere.velocity -= bullet->Velocity()*mass/(mass_-mass);
  sphere.angular_velocity = sphere.velocity / sphere.radius;
   bullet->UpdateTick();
  ChangeSize(sphere, pow((1 - mass / mass_), 1.0 / 3), mass);
}

void BigBall::UpdateTick() {
  float delta_time = world_->TickDeltaT();
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);

  auto owner = world_->GetPlayer(player_id_);
  if (owner) {
    if (UnitId() == owner->PrimaryUnitId()) {
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
      if (input.add_size) {
        ChangeSize(sphere,1.1);
      }
      if (input.minus_size) {
        ChangeSize(sphere, 0.9);
      }
      if (input.split) {//input.split
        uint64_t nw=world_->Version();
        if (nw - past >= 100) {
          std::mt19937 gen;
          std::uniform_real_distribution<float> rd(0.5f, 1.0f);
          float radius = rd(gen) / 1.5f*radius_;
          float mass = mass_ * pow(radius / radius_, 3);
          Split(sphere,mass,radius);
          past = nw; 
        }
      }
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

  sphere.velocity *= std::pow(0.5f, delta_time);
  sphere.angular_velocity *= std::pow(0.2f, delta_time);

  position_ = sphere.position;
  velocity_ = sphere.velocity;
  orientation_ = sphere.orientation;
  augular_momentum_ = sphere.inertia * sphere.angular_velocity;
}

void BigBall::SetMass(float mass) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius_, mass);
  mass_ = mass;
}

void BigBall::SetGravity(const glm::vec3 &gravity) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.gravity = gravity;
}

void BigBall::SetRadius(float radius) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius, mass_);
  radius_ = radius;
}

void BigBall::SetMotion(const glm::vec3 &position,
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

glm::vec3 BigBall::Position() const {
  return position_;
}

glm::vec3 BigBall::Velocity() const {
  return velocity_;
}

glm::mat3 BigBall::Orientation() const {
  return orientation_;
}

glm::vec3 BigBall::AngularMomentum() const {
  return augular_momentum_;
}

}  // namespace GameBall::Logic::Units
