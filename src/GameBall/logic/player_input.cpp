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

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
      input_.jump_released) {
    input_.jump = true;
    input_.jump_released = false;
  } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
    input_.jump_released = true;
  } else {
    input_.jump = false;
  }

  auto result = input_;
  input_ = {};
  return result;
}

bool operator==(const Logic::PlayerInput &lhs, const Logic::PlayerInput &rhs) {
  // Compare each member of Logic::PlayerInput
  return lhs.move_forward == rhs.move_forward &&
         lhs.move_backward == rhs.move_backward &&
         lhs.move_left == rhs.move_left && lhs.move_right == rhs.move_right &&
         lhs.brake == rhs.brake && lhs.jump == rhs.jump &&
         lhs.jump_released == rhs.jump_released;
}

bool operator!=(const Logic::PlayerInput &lhs, const Logic::PlayerInput &rhs) {
  // Compare each member of Logic::PlayerInput
  return lhs.move_forward != rhs.move_forward ||
         lhs.move_backward != rhs.move_backward ||
         lhs.move_left != rhs.move_left || lhs.move_right != rhs.move_right ||
         lhs.brake != rhs.brake || lhs.jump != rhs.jump ||
         lhs.jump_released != rhs.jump_released;
}

}  // namespace GameBall::Logic
