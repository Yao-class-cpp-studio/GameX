#pragma once
#include "GameBall/core/utils.h"
#include "GameBall/logic/player_input.h"

namespace GameBall::Logic {
class World;
class Player {
 public:
  Player(World *world);
  ~Player();
  uint64_t PlayerId() const;

  uint64_t PrimaryUnitId() const;

  void SetPrimaryUnit(uint64_t unit_id);

  void SetInput(const PlayerInput &input);

  [[nodiscard]] PlayerInput GetPlayerInput() const;

  PlayerInput TakePlayerInput();

  const std::string &GetIp() const;

  const uint16_t &GetPort() const;

  void SetNetInfo(const std::string &ip, const uint16_t &port);

 private:
  World *world_;
  uint64_t player_id_{};

  uint64_t primary_unit_id_{};

  PlayerInput input_{};

  std::string ip_;

  uint16_t port_;
};
}  // namespace GameBall::Logic
