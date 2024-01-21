#include "GameBall/logic/player.h"

#include "GameBall/logic/world.h"

namespace GameBall::Logic {

Player::Player(World *world) : world_(world) {
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

uint64_t Player::LargeUnitId() const {
  return large_unit_id_;
} 

uint64_t Player::SmallUnitId() const {
  return small_unit_id_;
}

void Player::SetPrimaryUnit(uint64_t unit_id) {
  primary_unit_id_ = unit_id;
}

void Player::SetLargeUnit(uint64_t unit_id) {
  large_unit_id_ = unit_id;
}

void Player::SetSmallUnit(uint64_t unit_id) {
  small_unit_id_ = unit_id;
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
}  // namespace GameBall::Logic
