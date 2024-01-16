#pragma once

#include <functional>
#include <queue>

#include "GameBall/core/utils.h"
#include "GameBall/logic/object.h"
#include "GameBall/logic/obstacle.h"
#include "GameBall/logic/player.h"
#include "GameBall/logic/unit.h"

namespace GameBall::Logic {
class Manager;

class Object;

class Unit;

class Obstacle;

class Player;

class World {
 public:
  World();

  ~World();

  GameX::Physics::World *PhysicsWorld() const;

  uint64_t RegisterObject(Object *object);

  void UnregisterObject(uint64_t object_id);

  uint64_t RegisterUnit(Unit *unit);

  void UnregisterUnit(uint64_t unit_id);

  uint64_t RegisterObstacle(Obstacle *obstacle);

  void UnregisterObstacle(uint64_t obstacle_id);

  uint64_t RegisterPlayer(Player *player);

  void UnregisterPlayer(uint64_t player_id);

  uint64_t Version() const {
    return world_version_;
  }

  void SyncWorldState(GameBall *app) const;

  Object *GetObject(uint64_t object_id) const;

  Unit *GetUnit(uint64_t unit_id) const;

  Obstacle *GetObstacle(uint64_t obstacle_id) const;

  Player *GetPlayer(uint64_t player_id) const;

  template <class UnitType, class... Args>
  UnitType *CreateUnit(uint64_t player_id, Args &&...args) {
    auto unit = new UnitType(this, player_id, std::forward<Args>(args)...);
    return unit;
  }

  template <class ObstacleType, class... Args>
  ObstacleType *CreateObstacle(Args &&...args) {
    auto obstacle = new ObstacleType(this, std::forward<Args>(args)...);
    return obstacle;
  }

  Player *CreatePlayer() {
    auto player = new Player(this);
    return player;
  }

  bool RemovePlayer(uint64_t player_id);

  bool RemoveUnit(uint64_t unit_id);

  bool RemoveObstacle(uint64_t obstacle_id);

  void PushEvent(const std::function<void()> &event) {
    world_events_.push(event);
  }

  float TickDeltaT() const {
    return 1.0f / 64.0f;
  }

  void UpdateTick();

  int restart = 0;

  int score[2] = {0, 0};

 private:
  friend ::GameBall::GameBall;
  friend ::GameBall::Logic::Manager;

  std::unique_ptr<GameX::Physics::World> physics_world_;
  uint64_t next_object_id_{1};
  std::map<uint64_t, Object *> object_map_;

  uint64_t next_unit_id_{1};
  std::map<uint64_t, Unit *> unit_map_;

  uint64_t next_obstacle_id_{1};
  std::map<uint64_t, Obstacle *> obstacle_map_;

  uint64_t next_player_id_{1};
  std::map<uint64_t, Player *> player_map_;

  uint64_t world_version_{1};

  std::queue<std::function<void()>> world_events_;
};
}  // namespace GameBall::Logic
