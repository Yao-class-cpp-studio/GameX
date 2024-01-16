#include "GameBall/core/actors/common_ball_actor.h"

#include "GameBall/core/game_ball.h"

namespace GameBall::Actors {

CommonBallActor::CommonBallActor(GameBall *app) : Actor(app) {
  entity_ = app->Scene()->CreateEntity(
      app->AssetManager()->StaticModelFile("models/sphere.obj"));
  entity_->SetAlbedoImage(
      app->AssetManager()->ImageFile("textures/2k_moon.jpg"));
}

CommonBallActor::~CommonBallActor() {
  if (entity_) {
    app_->Scene()->DestroyEntity(entity_.get());
    entity_ = nullptr;
  }
}

void CommonBallActor::Update(float delta_time) {
  glm::vec3 acceleration = gravity_;
  velocity_ += acceleration * delta_time;
  position_ += velocity_ * delta_time;

  glm::vec3 omega = rotation_ * J_inv_ * (L_ * rotation_);
  float theta = glm::length(omega) * delta_time;
  if (theta > 0.0f) {
    rotation_ =
        glm::mat3{glm::rotate(glm::mat4{1.0f}, theta, omega)} * rotation_;
  }

  entity_->SetAffineMatrix(glm::translate(glm::mat4{1.0f}, position_) *
                           glm::mat4{rotation_} * glm::mat4{transform_});
}

void CommonBallActor::SetMomentOfInertia(float moment_of_inertia) {
  J_ = glm::mat3{moment_of_inertia};
  J_inv_ = glm::mat3{1.0f / moment_of_inertia};
}

void CommonBallActor::SetInertiaTensor(const glm::mat3 &inertia_tensor) {
  J_ = inertia_tensor;
  J_inv_ = glm::inverse(inertia_tensor);
}

void CommonBallActor::SetMass(float mass) {
  mass_ = mass;
}

void CommonBallActor::SetGravity(const glm::vec3 &gravity) {
  gravity_ = gravity;
}

void CommonBallActor::SetTransform(const glm::mat3 &transform) {
  transform_ = transform;
}

void CommonBallActor::SetMotion(const glm::vec3 &position,
                                const glm::vec3 &velocity,
                                const glm::mat3 &rotation,
                                const glm::vec3 &angular_momentum) {
  rotation_ = rotation;
  position_ = position;
  velocity_ = velocity;
  L_ = angular_momentum;
}

}  // namespace GameBall::Actors
