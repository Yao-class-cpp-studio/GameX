#include "GameBall/logic/player_input.h"

#include "GameBall/core/game_ball.h"

namespace GameBall::Logic {
PlayerInputController::PlayerInputController(GameBall *app) : app_(app) {
}

PlayerInput PlayerInputController::GetInput() {
  auto window = app_->Window();
  input_.move_forward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
  input_.move_backward = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
  input_.move_left = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
  input_.move_right = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
  input_.brake = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
  auto camera_controller = app_->CameraController();
  auto pitch_yaw = camera_controller->GetPitchYaw();
  auto pitch = pitch_yaw.x;
  auto yaw = pitch_yaw.y;
  input_.orientation = {glm::sin(glm::radians(yaw)), 0.0f,
                        -glm::cos(glm::radians(yaw))};
  input_.v_jump = (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS);
  input_.low = (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS);
  input_.high = (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS);
   // 2 chances of returning to the base
  //changed to enable the judgement of winning or not.
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    R_pressed_now = R_pressed_before = true;
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && R_pressed_before&&R_pressed_now) {
    count_chances++;
   R_pressed_now = false;
  }
  input_.return_if_too_light = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS&&count_chances<2);
  input_.halt = (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS);
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    T_pressed_now = true;
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && T_pressed_now)
    T_pressed_now = !T_pressed_now, T_pressed_before = !T_pressed_before;
  input_.restart_halt = T_pressed_before;
  input_.end_if_too_heavy = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
  input_.end = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
  auto result = input_;
  input_ = {};
  return result;
}

}  // namespace GameBall::Logic
