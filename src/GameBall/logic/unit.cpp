#include "GameBall/logic/unit.h"

#include "GameBall/logic/world.h"

namespace GameBall::Logic {
Unit::Unit(World *world, uint64_t player_id)
    : Object(world), player_id_(player_id) {
  unit_id_ = world_->RegisterUnit(this);
}

Unit::~Unit() {
  world_->UnregisterUnit(unit_id_);
}

uint64_t Unit::UnitId() const {
  return unit_id_;
}

uint64_t Unit::PlayerId() const {
  return player_id_;
}
void Unit::SetEnemy(uint64_t en) {
  enemy_id_ = en;
}
}  // namespace GameBall::Logic

