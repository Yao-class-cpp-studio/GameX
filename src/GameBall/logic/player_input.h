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
  bool v_jump{false};  // make the sphere jump if it is on the cube
  bool low{false};     // lower the mass by 10% once called
  bool high{false};    // increase the mass by 10% at the supply base(0,1,0)
  bool return_if_too_light{
      false};  // return to the supply base and recover normal mass if called
  bool halt{false};  // Stop the game and the balls will make no response until
                     // halting ends
  bool restart_halt{false};  // Halt by pressing T once, and restart the game by
                             // pressing T again
  bool end_if_too_heavy{false};  // end the game if the ball is too heavy
  bool end{false};               // quit immediately
};

class PlayerInputController {
 public:
  PlayerInputController(GameBall *app);
  PlayerInput GetInput();

 private:
  GameBall *app_;
  PlayerInput input_{};
  bool R_pressed_before = false;  // indicate whether R has been pressed before
  bool R_pressed_now = false;     // indicate whether R is being pressed
  bool T_pressed_before = false;
  bool T_pressed_now = false;
  int count_chances = 0, count_halt = 0;
};

}  // namespace GameBall::Logic
