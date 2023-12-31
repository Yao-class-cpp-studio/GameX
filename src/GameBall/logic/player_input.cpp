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
<<<<<<< Updated upstream
=======
  input_.sprint = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
  input_.ball_large = (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS);
  input_.ball_small = (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS);
  input_.mass_large = (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS);
  input_.mass_small = (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS);
>>>>>>> Stashed changes
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
