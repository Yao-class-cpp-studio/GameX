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
  auto result = input_;
  input_ = {};
  return result;
}

PlayerInput::operator std::string() const {
  std::string str = "";
  if (move_forward) str += '1'; else str += '0';
  if (move_backward) str += '1'; else str += '0';
  if (move_left) str += '1'; else str += '0';
  if (move_right) str += '1'; else str += '0';
  if (brake) str += '1'; else str += '0';
  for (int i = 3; i >= -4; --i) str += char(int(orientation.x / pow(10, i)) % 10 + '0');
  for (int i = 3; i >= -4; --i) str += char(int(orientation.y / pow(10, i)) % 10 + '0');
  for (int i = 3; i >= -4; --i) str += char(int(orientation.z / pow(10, i)) % 10 + '0');
  return str;
}

PlayerInput::PlayerInput(std::string str) {
  if (str != ""){
    move_forward = bool(str[0] - '0');
    move_backward = bool(str[1] - '0');
    move_left = bool(str[2] - '0');
    move_right = bool(str[3] - '0');
    brake = bool(str[4] - '0');
    float x = 0, y = 0, z = 0;
    for (int i = 0; i < 8; ++i) {
      x *= 10;
      x += int(str[5 + i] - '0');
    }
    for (int i = 0; i < 8; ++i) {
      y *= 10;
      y += int(str[13 + i] - '0');
    }
    for (int i = 0; i < 8; ++i) {
      z *= 10;
      z += int(str[21 + i] - '0');
    }
    orientation.x = x / 10000;
    orientation.y = y / 10000;
    orientation.z = z / 10000;
  }
}

}  // namespace GameBall::Logic
