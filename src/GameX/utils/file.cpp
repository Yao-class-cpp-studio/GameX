#include "GameX/utils/file.h"

namespace GameX::Base {

bool FileExists(const std::string &path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}
}  // namespace GameX::Base
