#pragma once

#include "GameBall/core/actors/common_ball_actor.h"
#include "GameBall/logic/unit.h"

namespace GameBall::Logic::Units {
class RegularBall : public Unit {
 public:
  RegularBall(World *world,
              uint64_t player_id,
              const glm::vec3 &position,
              float radius = 1.0f,
              float mass = 1.0f);

  ~RegularBall();

  SYNC_ACTOR_FUNC_DECLARE(Actors::CommonBallActor)

  void UpdateTick() override;

  void SetMass(float mass);
  void SetGravity(const glm::vec3 &gravity);
  void SetRadius(float radius);
  void SetMotion(const glm::vec3 &position = glm::vec3{0.0f},
                 const glm::vec3 &velocity = glm::vec3{0.0f},
                 const glm::mat3 &orientation = glm::mat3{1.0f},
                 const glm::vec3 &angular_momentum = glm::vec3{0.0f});

  glm::vec3 Position() const;
  glm::vec3 Velocity() const;
  glm::mat3 Orientation() const;
  glm::vec3 AngularMomentum() const;

 private:
  float radius_{1.0f};
  float mass_{1.0f};
  glm::vec3 position_{};
  glm::vec3 velocity_{};
  glm::mat3 orientation_{1.0f};
  glm::vec3 augular_momentum_{};
  uint64_t sphere_id_{};
};
}  // namespace GameBall::Logic::Units
