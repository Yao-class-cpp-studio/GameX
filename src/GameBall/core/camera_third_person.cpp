#include "GameBall/core/camera_third_person.h"

namespace GameBall {

CameraControllerThirdPerson::CameraControllerThirdPerson(
    GameX::Graphics::PCamera camera,
    float aspect) {
  camera_ = camera;
  aspect_ = aspect;
}

void CameraControllerThirdPerson::SetCenter(const glm::vec3 &center) {
  dst_center_ = center;
}

void CameraControllerThirdPerson::SetPitchYaw(float pitch, float yaw) {
  dst_pitch_ = pitch;
  dst_yaw_ = yaw;
}

void CameraControllerThirdPerson::SetDistance(float distance) {
  dst_distance_ = distance;
}

void CameraControllerThirdPerson::SetFovY(float fovY) {
  dst_fovY_ = fovY;
}

glm::vec2 CameraControllerThirdPerson::GetPitchYaw() const {
  float diff_pitch = dst_pitch_ - src_pitch_;
  float diff_yaw = dst_yaw_ - src_yaw_;
  diff_pitch = glm::mod(diff_pitch + 180.0f, 360.0f) - 180.0f;
  diff_yaw = glm::mod(diff_yaw + 180.0f, 360.0f) - 180.0f;
  float new_pitch = src_pitch_ + diff_pitch * interpolarion_factor_;
  float new_yaw = src_yaw_ + diff_yaw * interpolarion_factor_;
  return glm::vec2{new_pitch, new_yaw};
}

glm::vec3 CameraControllerThirdPerson::GetCenter() const {
  return src_center_ + (dst_center_ - src_center_) * interpolarion_factor_;
}

void CameraControllerThirdPerson::SetInterpolationFactor(float factor) {
  interpolarion_factor_ = factor;
}

float CameraControllerThirdPerson::GetDistance() const {
  return src_distance_ +
         (dst_distance_ - src_distance_) * interpolarion_factor_;
}

float CameraControllerThirdPerson::GetInterpolationFactor() const {
  return interpolarion_factor_;
}

float CameraControllerThirdPerson::GetFovY() const {
  return src_fovY_ + (dst_fovY_ - src_fovY_) * interpolarion_factor_;
}

void CameraControllerThirdPerson::StoreCurrentState() {
  src_center_ = GetCenter();
  auto pitch_yaw = GetPitchYaw();
  src_pitch_ = pitch_yaw.x;
  src_yaw_ = pitch_yaw.y;
  src_distance_ = GetDistance();
  src_fovY_ = GetFovY();
}

void CameraControllerThirdPerson::Update(float delta_time) {
  interpolarion_factor_ += delta_time * 2.0f;
  interpolarion_factor_ = glm::clamp(interpolarion_factor_, 0.0f, 1.0f);
  glm::vec2 pitch_yaw = GetPitchYaw();
  glm::vec3 center = GetCenter();
  float pitch = pitch_yaw.x;
  float yaw = pitch_yaw.y;
  float distance_ = GetDistance();
  float fovY = GetFovY();
  glm::vec3 back{glm::cos(glm::radians(pitch)) * -glm::sin(glm::radians(yaw)),
                 glm::sin(glm::radians(pitch)),
                 glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw))};
  glm::vec3 right{glm::cos(glm::radians(yaw)), 0.0f,
                  glm::sin(glm::radians(yaw))};
  glm::vec3 up = glm::cross(back, right);

  glm::vec3 eye = center + back * distance_;

  glm::mat4 view_matrix{glm::vec4{right, 0.0f}, glm::vec4{up, 0.0f},
                        glm::vec4{back, 0.0f},
                        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}};
  view_matrix =
      glm::transpose(view_matrix) * glm::translate(glm::mat4{1.0f}, -eye);
  glm::mat4 projection_matrix =
      glm::perspectiveZO(glm::radians(fovY), aspect_, 0.1f, 100.0f);
  camera_->SetCameraData({view_matrix, projection_matrix});
}

void CameraControllerThirdPerson::CursorMove(float x, float y) {
  dst_yaw_ += x * 0.1f;
  dst_pitch_ += y * 0.1f;
  dst_pitch_ = glm::clamp(dst_pitch_, -89.0f, 89.0f);
}

void CameraControllerThirdPerson::CursorScroll(float x, float y) {
  float target_diatance_ = dst_distance_ + y;
  target_diatance_ = glm::clamp(target_diatance_, 5.0f, 35.0f);

  dst_distance_ += (target_diatance_ - dst_distance_) * 0.3f;
}

}  // namespace GameBall
