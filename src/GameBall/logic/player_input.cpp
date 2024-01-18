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
  input_.brake = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
  input_.jump = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
  input_.quit = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
  input_.reset = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
  input_.move_forward1 = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
  input_.move_backward1 = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
  input_.move_left1 = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
  input_.move_right1 = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
  input_.brake1 = (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
  input_.jump1 = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
  auto camera_controller = app_->CameraController();
  auto pitch_yaw = camera_controller->GetPitchYaw();
  auto pitch = pitch_yaw.x;
  auto yaw = pitch_yaw.y;
  input_.orientation = {glm::sin(glm::radians(yaw)), 0.0f,
                        -glm::cos(glm::radians(yaw))};
  auto result = input_;
  input_ = {};
  return result;
}

}  // namespace GameBall::Logic
