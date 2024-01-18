#pragma once

#include "GameBall/core/utils.h"

namespace GameBall {
class CameraControllerThirdPerson {
 public:
  CameraControllerThirdPerson(GameX::Graphics::PCamera camera, float aspect);

  void SetCenter(const glm::vec3 &center);

  void SetPitchYaw(float pitch, float yaw);

  void SetDistance(float distance);

  void SetFovY(float fovY);

  glm::vec2 GetPitchYaw() const;

  glm::vec3 GetCenter() const;

  float GetDistance() const;

  float GetFovY() const;

  void SetInterpolationFactor(float factor = 0.0f);

  float GetInterpolationFactor() const;

  void StoreCurrentState();

  void Update(float delta_time);

  void CursorMove(float x, float y);

 private:
  GameX::Graphics::PCamera camera_;
  float dst_distance_{25.0f};
  glm::vec3 dst_center_{0.0f};
  float dst_pitch_{0.0f};
  float dst_yaw_{0.0f};
  float dst_fovY_{30.0f};
  float src_distance_{10.0f};
  glm::vec3 src_center_{0.0f};
  float src_pitch_{0.0f};
  float src_yaw_{0.0f};
  float src_fovY_{30.0f};

  float aspect_{1.0f};

  float interpolarion_factor_{0.0f};
};
}  // namespace GameBall
