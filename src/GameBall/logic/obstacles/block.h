#pragma once

#include "GameBall/logic/obstacle.h"

namespace GameBall::Logic::Obstacles {
class Block : public Obstacle {
 public:
  Block(World *world,
        glm::vec3 position,
        float mass = std::numeric_limits<float>::infinity(),
        bool gravity = false,
        float side_length = 1.0f);

  SYNC_ACTOR_FUNC_DECLARE(Actors::CommonBlockActor)

  void SetMass(float mass);

  void SetGravity(const glm::vec3 &gravity);

  void SetSideLength(float side_length);

  void SetMotion(const glm::vec3 &position,
                 const glm::vec3 &velocity,
                 const glm::mat3 &orientation,
                 const glm::vec3 &angular_momentum);

  void UpdateTick() override;

 private:
  float side_length_{};
  glm::vec3 position_{};
  glm::vec3 velocity_{};

  glm::mat3 orientation_{1.0f};
  glm::vec3 L_{0.0f};  // angular momentum
  glm::mat3 J_{1.0f};  // inertia tensor

  glm::vec3 gravity_{};
  float mass_{};
  uint64_t timer_ = 0;

  uint64_t cude_id_{};
};
}  // namespace GameBall::Logic::Obstacles
