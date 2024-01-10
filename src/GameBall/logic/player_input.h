#pragma once
#include "GameBall/core/utils.h"

namespace GameBall {
class GameBall;
}

namespace GameBall::Logic {
struct PlayerInput {
  bool move_forward{false};
  bool move_backward{false};
  bool move_left{false};
  bool move_right{false};
  bool speed{false};
  bool jump{false};
  bool large{false};
  bool small{false};
  bool heavy{false};
  bool light{false};
  bool brake{false};
  bool no_damping{false};
  bool damping{false};
  glm::vec3 orientation{0.0f, 0.0f, 1.0f};
};

class PlayerInputController {
 public:
  PlayerInputController(GameBall *app);
  PlayerInput GetInput();

 private:
  GameBall *app_;
  PlayerInput input_{};
};

}  // namespace GameBall::Logic
