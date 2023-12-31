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
<<<<<<< Updated upstream
=======
  bool sprint{false};
  bool ball_large{false};
  bool ball_small{false};
  bool mass_large{false};
  bool mass_small{false};
>>>>>>> Stashed changes
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
