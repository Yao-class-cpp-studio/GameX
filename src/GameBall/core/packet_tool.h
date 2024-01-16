#include <cstring>
#include <sstream>
#ifndef GAMEX_PACKET_TOOL_H
#define GAMEX_PACKET_TOOL_H

namespace GameBall {

std::string PlayerInputToString(const Logic::PlayerInput &input) {
  std::string str;
  str += input.move_forward ? '1' : '0';
  str += input.move_backward ? '1' : '0';
  str += input.move_left ? '1' : '0';
  str += input.move_right ? '1' : '0';
  str += input.brake ? '1' : '0';
  str += input.jump ? '1' : '0';
  str += input.jump_released ? '1' : '0';

  std::ostringstream oss;
  oss << "(" << input.orientation.x << "," << input.orientation.y << ","
      << input.orientation.z << ")";
  str += oss.str();

  return str;
}

Logic::PlayerInput StringToPlayerInput(const std::string &str) {
  Logic::PlayerInput input;
  input.move_forward = (str[0] == '1');
  input.move_backward = (str[1] == '1');
  input.move_left = (str[2] == '1');
  input.move_right = (str[3] == '1');
  input.brake = (str[4] == '1');
  input.jump = (str[5] == '1');
  input.jump_released = (str[6] == '1');

  std::istringstream iss(str.substr(7));
  char left_bracket, comma1, comma2, right_bracket;
  iss >> left_bracket >> input.orientation.x >> comma1 >> input.orientation.y >>
      comma2 >> input.orientation.z >> right_bracket;

  return input;
}

// Credit to Yuchen Yang
// Ball State Compression Module
// Convert a 18*float array to a 96-base64 string

#define BLOCK_SIZE 12
char _str_id[255], _str_tp[64];
void base64Init() {
  for (int i = 'A'; i <= 'Z'; ++i)
    _str_tp[_str_id[i] = i - 'A'] = i;
  for (int i = 'a'; i <= 'z'; ++i)
    _str_tp[_str_id[i] = i - 'a' + 26] = i;
  for (int i = '0'; i <= '9'; ++i)
    _str_tp[_str_id[i] = i - '0' + 52] = i;
  _str_tp[_str_id['+'] = 62] = '+';
  _str_tp[_str_id['/'] = 63] = '/';
}
std::string encode_state(float *data) {
  uint8_t *p = (uint8_t *)data;
  std::string q;
  for (int i = 0; i < BLOCK_SIZE; i++) {
    uint64_t now = 0;
    for (int j = 0; j < 6; j++)
      now = now << 8 | p[i * 6 + j];
    for (int j = 0; j < 8; j++)
      q += _str_tp[now >> (j * 6) & 63];
  }
  return q;
}
void decode_state(std::string s, float *ret) {
  uint8_t *p = (uint8_t *)ret;
  for (int i = 0; i < BLOCK_SIZE; i++) {
    uint64_t now = 0;
    for (int j = 7; j >= 0; j--)
      now = now << 6 | _str_id[s[i * 8 + j]];
    for (int j = 0; j < 6; j++)
      p[i * 6 + 5 - j] = now >> (j * 8) & 255;
  }
}

}  // namespace GameBall

#endif  // GAMEX_PACKET_TOOL_H
