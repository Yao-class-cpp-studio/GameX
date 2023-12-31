#include "GameX/utils/utils.h"

namespace GameX::Base {

glm::mat3 Rotate(const glm::vec3 &axis, float radians) {
  float c = cos(radians);
  float s = sin(radians);
  float t = 1.0f - c;
  float x = axis.x;
  float y = axis.y;
  float z = axis.z;
  return glm::mat3(t * x * x + c, t * x * y + z * s, t * x * z - y * s,
                   t * x * y - z * s, t * y * y + c, t * y * z + x * s,
                   t * x * z + y * s, t * y * z - x * s, t * z * z + c);
}

glm::mat3 Rotate(const glm::vec3 &rotation) {
  float theta = glm::length(rotation);
  if (theta < 0.0001f) {
    return glm::mat3(1.0f);
  }
  return Rotate(rotation / theta, theta);
}

}  // namespace GameX::Base
