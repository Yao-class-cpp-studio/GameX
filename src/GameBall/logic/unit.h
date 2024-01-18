#pragma once
#include "GameBall/logic/object.h"

namespace GameBall::Logic {
class Unit : public Object {
 public:
  Unit(World *world, uint64_t player_id);
  virtual ~Unit();
  uint64_t UnitId() const;
  virtual void SetEnemy(uint64_t en);
  uint64_t PlayerId() const;
 private:
  uint64_t unit_id_{};

 protected:
  uint64_t player_id_{};
  uint64_t enemy_id_{};
};
}  // namespace GameBall::Logic
