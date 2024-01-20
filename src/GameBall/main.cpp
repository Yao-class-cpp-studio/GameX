#include "GameBall/core/game_ball.h"
#include "GameBall/core/p2pnode.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

// Use abseil flags to parse command line arguments.
ABSL_FLAG(bool, fullscreen, false, "Run in fullscreen mode.");
// Width and Height
ABSL_FLAG(int, width, -1, "Width of the window.");
ABSL_FLAG(int, height, -1, "Height of the window.");
ABSL_FLAG(std::string, mode, "local","Game mode");
ABSL_FLAG(std::string, addr, "","Server address");
ABSL_FLAG(int, port, DEFAULT_PORT, "Server Port");

int main(int argc, char *argv[]) {
  absl::ParseCommandLine(argc, argv);

  GameBall::GameSettings settings;
  settings.fullscreen = absl::GetFlag(FLAGS_fullscreen);
  settings.width = absl::GetFlag(FLAGS_width);
  settings.height = absl::GetFlag(FLAGS_height);
  settings.mode = absl::GetFlag(FLAGS_mode);
  settings.address = absl::GetFlag(FLAGS_addr);
  settings.port = absl::GetFlag(FLAGS_port);
  GameBall::GameBall game(settings);
  game.Run();
  return 0;
}
