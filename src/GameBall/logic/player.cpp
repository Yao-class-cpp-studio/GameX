#include "GameBall/logic/player.h"

#include "GameBall/logic/world.h"

namespace GameBall::Logic {

Player::Player(World *world) : world_(world), port_(DEFAULT_PORT) {
  player_id_ = world_->RegisterPlayer(this);
}

Player::~Player() {
  world_->UnregisterPlayer(player_id_);
}

uint64_t Player::PlayerId() const {
  return player_id_;
}

uint64_t Player::PrimaryUnitId() const {
  return primary_unit_id_;
}

void Player::SetPrimaryUnit(uint64_t unit_id) {
  primary_unit_id_ = unit_id;
}

void Player::SetInput(const PlayerInput &input) {
  input_ = input;
}

PlayerInput Player::GetPlayerInput() const {
  return input_;
}

PlayerInput Player::TakePlayerInput() {
  auto input = input_;
  input_ = {};
  return input;
}

const std::string &Player::GetIp() const {
  return ip_;
}

const uint16_t &Player::GetPort() const {
  return port_;
}

void Player::setNetInfo(const std::string &ip, const uint16_t &port) {
  ip_ = ip;
  port_ = port;
}
}  // namespace GameBall::Logic
