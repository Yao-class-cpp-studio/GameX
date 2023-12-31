#pragma once
#include "GameX/utils/asset_probe.h"
#include "GameX/utils/common.h"
#include "GameX/utils/file.h"
#include "GameX/utils/image.h"
#include "GameX/utils/mesh.h"
#include "GameX/utils/metronome.h"
#include "GameX/utils/vertex.h"

namespace GameX::Base {
glm::mat3 Rotate(const glm::vec3 &axis, float radians);

glm::mat3 Rotate(const glm::vec3 &rotation);
}  // namespace GameX::Base
