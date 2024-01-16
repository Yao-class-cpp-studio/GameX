#include "GameBall/logic/world.h"

#include "GameBall/core/game_ball.h"

extern bool is_server;

namespace GameBall::Logic {

World::World() {
  physics_world_ = std::make_unique<GameX::Physics::World>();
}

World::~World() {
  std::queue<Object *> object_queue;

  for (auto &pair : object_map_) {
    object_queue.push(pair.second);
  }

  while (!object_queue.empty()) {
    auto object = object_queue.front();
    object_queue.pop();
    delete object;
  }

  std::queue<Player *> player_queue;

  for (auto &pair : player_map_) {
    player_queue.push(pair.second);
  }

  while (!player_queue.empty()) {
    auto player = player_queue.front();
    player_queue.pop();
    delete player;
  }
}

GameX::Physics::World *World::PhysicsWorld() const {
  return physics_world_.get();
}

uint64_t World::RegisterObject(Object *object) {
  object_map_.insert(std::make_pair(next_object_id_, object));
  return next_object_id_++;
}

void World::UnregisterObject(uint64_t object_id) {
  object_map_.erase(object_id);
}

uint64_t World::RegisterUnit(Unit *unit) {
  unit_map_.insert(std::make_pair(next_unit_id_, unit));
  return next_unit_id_++;
}

void World::UnregisterUnit(uint64_t unit_id) {
  unit_map_.erase(unit_id);
}

uint64_t World::RegisterObstacle(Obstacle *obstacle) {
  obstacle_map_.insert(std::make_pair(next_obstacle_id_, obstacle));
  return next_obstacle_id_++;
}

void World::UnregisterObstacle(uint64_t obstacle_id) {
  obstacle_map_.erase(obstacle_id);
}

uint64_t World::RegisterPlayer(Player *player) {
  player_map_.insert(std::make_pair(next_player_id_, player));
  return next_player_id_++;
}

void World::UnregisterPlayer(uint64_t player_id) {
  player_map_.erase(player_id);
}

void World::UpdateTick() {
  // LAND_INFO("Update Tick... {}", world_version_);
  if (is_server) {
    physics_world_->ApplyGravity(TickDeltaT());
    physics_world_->SolveCollisions();
  }

  for (auto &pair : object_map_) {
    pair.second->UpdateTick();
  }


  if (is_server) {
    physics_world_->SolveCollisions();
  }

  physics_world_->Update(TickDeltaT());

  world_version_++;
}

void World::SyncWorldState(GameBall *app) const {
  if (app->synced_logic_world_version_ == world_version_) {
    return;
  }

  app->synced_logic_world_version_ = world_version_;
  for (auto &pair : object_map_) {
    pair.second->SyncState(app);
    app->actors_.at(pair.first)->synced_logic_world_version_ = world_version_;
    pair.second->actor_initialize_ = false;
  }
}

Object *World::GetObject(uint64_t object_id) const {
  if (!object_map_.count(object_id)) {
    return nullptr;
  }
  return object_map_.at(object_id);
}

Unit *World::GetUnit(uint64_t unit_id) const {
  if (!unit_map_.count(unit_id)) {
    return nullptr;
  }
  return unit_map_.at(unit_id);
}

Obstacle *World::GetObstacle(uint64_t obstacle_id) const {
  if (!obstacle_map_.count(obstacle_id)) {
    return nullptr;
  }
  return obstacle_map_.at(obstacle_id);
}

Player *World::GetPlayer(uint64_t player_id) const {
  if (!player_map_.count(player_id)) {
    return nullptr;
  }
  return player_map_.at(player_id);
}

bool World::RemovePlayer(uint64_t player_id) {
  auto player = GetPlayer(player_id);
  if (player == nullptr) {
    return false;
  }
  delete player;
  return true;
}

bool World::RemoveUnit(uint64_t unit_id) {
  auto unit = GetUnit(unit_id);
  if (unit == nullptr) {
    return false;
  }
  delete unit;
  return true;
}

bool World::RemoveObstacle(uint64_t obstacle_id) {
  auto obstacle = GetObstacle(obstacle_id);
  if (obstacle == nullptr) {
    return false;
  }
  delete obstacle;
  return true;
}
}  // namespace GameBall::Logic
