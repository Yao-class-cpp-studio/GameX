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
  bool R_pressed_before = false;  // indicate whether R has been pressed before
  bool R_pressed_now = false;
  bool T_pressed_before = false;
  bool T_pressed_now = false;
  int count_chances = 0, count_halt = 0;
};

}  // namespace GameBall::Logic
