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
  bool rotate_z{false};
  bool move_up{false};
  bool easy_mode{false};
  bool medium_mode{false};
  bool difficult_mode{false};
  bool original_mode{false};
  bool brake{false};
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
