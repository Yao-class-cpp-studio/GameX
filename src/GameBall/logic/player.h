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

  uint64_t LargeUnitId() const;

  uint64_t SmallUnitId() const;

  void SetPrimaryUnit(uint64_t unit_id);

  void SetLargeUnit(uint64_t unit_id);

  void SetSmallUnit(uint64_t unit_id);

  void SetInput(const PlayerInput &input);

  [[nodiscard]] PlayerInput GetPlayerInput() const;

  PlayerInput TakePlayerInput();

 private:
  World *world_;
  uint64_t player_id_{};

  uint64_t primary_unit_id_{};
  uint64_t large_unit_id_{};
  uint64_t small_unit_id_{};

  PlayerInput input_{};
};
}  // namespace GameBall::Logic
