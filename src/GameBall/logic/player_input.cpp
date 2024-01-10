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
  input_.speed = (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS);
  input_.jump = (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS);
  input_.large = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
  input_.small = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
  input_.heavy = (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS);
  input_.light = (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
  input_.no_damping = (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS);
  input_.damping = (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS);
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
