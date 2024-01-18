#include <cstring>
#include <sstream>
#include "GameBall/logic/logic.h"

#ifndef GAMEX_PACKET_TOOL_H
#define GAMEX_PACKET_TOOL_H

namespace GameBall {

std::string PlayerInputToString(const Logic::PlayerInput &input);

Logic::PlayerInput StringToPlayerInput(const std::string &str);

// Credit to Yuchen Yang
// Ball State Compression Module
// Convert a 18*float array to a 96-base64 string

#define BLOCK_SIZE 12

void base64Init();

std::string encode_state(float *data);

void decode_state(std::string s, float *ret);

}  // namespace GameBall

#endif  // GAMEX_PACKET_TOOL_H
