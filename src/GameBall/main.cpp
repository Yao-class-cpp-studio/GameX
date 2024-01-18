#include "GameBall/core/game_ball.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "GameBall/core/p2pnode.h"

#define DEFAULT_PORT 1115
#define DEFAULT_SELF_PORT 1116

// Use abseil flags to parse command line arguments.

ABSL_FLAG(bool, fullscreen, false, "Run in fullscreen mode.");

// Width and Height
ABSL_FLAG(int, width, -1, "Width of the window.");
ABSL_FLAG(int, height, -1, "Height of the window.");
ABSL_FLAG(std::string, mode, "", "Game mode");
ABSL_FLAG(std::string, address, "", "Server address");
ABSL_FLAG(int, port, DEFAULT_PORT, "Server Port");
ABSL_FLAG(int, self_port, DEFAULT_SELF_PORT, "Server Port");

uint8_t app_type;
// 0: room, 1: client, 2: local

int main(int argc, char *argv[]) {
  absl::ParseCommandLine(argc, argv);

  GameBall::GameSettings settings;
  settings.fullscreen = absl::GetFlag(FLAGS_fullscreen);
  settings.width = absl::GetFlag(FLAGS_width);
  settings.height = absl::GetFlag(FLAGS_height);
  settings.mode = absl::GetFlag(FLAGS_mode);
  settings.address = absl::GetFlag(FLAGS_address);
  settings.port = absl::GetFlag(FLAGS_port);

  if (settings.mode == "room") {
    app_type = 0;
  } else if (settings.mode == "client") {
    app_type = 1;
  } else if (settings.mode == "local" || settings.mode == ""){
    app_type = 2;
  } else {
    std::cout << "Invalid mode" << std::endl;
    exit(0);
  }
  GameBall::GameBall game(settings);
  game.Run();
  return 0;
}
