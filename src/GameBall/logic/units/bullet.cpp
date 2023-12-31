#include "GameBall/logic/units/bullet.h"

#include "GameBall/core/game_ball.h"
#include "GameBall/logic/world.h"

namespace GameBall::Logic::Units {
Bullet::Bullet(World *world,
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

Bullet::~Bullet() {
  ;
}

SYNC_ACTOR_FUNC(Bullet) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  actor->SetMass(1.0f);
  actor->SetGravity(glm::vec3{0.0f, -9.8f, 0.0f});
  actor->SetTransform(glm::mat3{radius_});
  actor->SetMotion(position_, velocity_, orientation_, augular_momentum_);
  actor->SetMomentOfInertia(sphere.inertia[0][0]);
}

void Bullet::UpdateTick() {
  float delta_time = world_->TickDeltaT();
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);

  sphere.velocity *= std::pow(0.99999f, delta_time);
  sphere.angular_velocity *= std::pow(0.99999f, delta_time);

  position_ = sphere.position;
  velocity_ = sphere.velocity;
  orientation_ = sphere.orientation;
  augular_momentum_ = sphere.inertia * sphere.angular_velocity;
}

void Bullet::SetMass(float mass) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius_, mass);
  mass_ = mass;
}

void Bullet::SetGravity(const glm::vec3 &gravity) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.gravity = gravity;
}

void Bullet::SetRadius(float radius) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius, mass_);
  radius_ = radius;
}

void Bullet::SetMotion(const glm::vec3 &position,
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

glm::vec3 Bullet::Position() const {
  return position_;
}

glm::vec3 Bullet::Velocity() const {
  return velocity_;
}

glm::mat3 Bullet::Orientation() const {
  return orientation_;
}

glm::vec3 Bullet::AngularMomentum() const {
  return augular_momentum_;
}

}  // namespace GameBall::Logic::Units
