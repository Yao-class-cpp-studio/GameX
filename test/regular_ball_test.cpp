#include <gtest/gtest.h>

#include <random>

#include "GameBall/core/game_ball.h"
#include "GameBall/logic/obstacles/obstacles.h"
#include "GameBall/logic/units/units.h"

uint8_t app_type;

void TestPlayerInput(GameBall::Logic::PlayerInput input,
                     glm::vec3 expected_position) {
  GameBall::Logic::World world;

  uint64_t player_id{};
  uint64_t unit_id{};

  {
    auto player = world.CreatePlayer();
    EXPECT_NE(player, nullptr);
    player_id = player->PlayerId();

    auto unit = world.CreateUnit<GameBall::Logic::Units::RegularBall>(
        player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);
    unit_id = unit->UnitId();
    player->SetPrimaryUnit(unit->UnitId());

    EXPECT_NE(unit, nullptr);

    auto obstacle = world.CreateObstacle<GameBall::Logic::Obstacles::Block>(
        glm::vec3{0.0f, -50.0f, 0.0f}, std::numeric_limits<float>::infinity(),
        false, 100.0f);

    EXPECT_NE(obstacle, nullptr);
  }

  const auto max_tick = 500;

  for (int i = 0; i < max_tick; ++i) {
    auto player = world.GetPlayer(player_id);
    EXPECT_NE(player, nullptr);
    player->SetInput(input);
    world.UpdateTick();
    auto unit = world.GetUnit(unit_id);
    EXPECT_NE(unit, nullptr);
    auto regular_ball =
        dynamic_cast<GameBall::Logic::Units::RegularBall *>(unit);
    EXPECT_NE(regular_ball, nullptr);

    if (glm::length(regular_ball->Position() - expected_position) < 0.2f) {
      SUCCEED();
      return;
    }
  }

  FAIL();
}

TEST(RegularBall, Functional) {
  app_type = 0;
  std::random_device rd;
  std::mt19937 gen(rd());
  for (int i = 0; i < 100; i++) {
    GameBall::Logic::PlayerInput input;
    float yaw = std::uniform_real_distribution<float>(0.0f, 360.0f)(gen);
    glm::vec3 forward = glm::normalize(glm::vec3{
        glm::cos(glm::radians(yaw)), 0.0f, glm::sin(glm::radians(yaw))});
    glm::vec3 right =
        glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));
    input.orientation = forward;
    input.move_forward = std::uniform_int_distribution<int>(0, 1)(gen);
    input.move_backward = std::uniform_int_distribution<int>(0, 1)(gen);
    input.move_left = std::uniform_int_distribution<int>(0, 1)(gen);
    input.move_right = std::uniform_int_distribution<int>(0, 1)(gen);

    glm::vec2 combined = glm::vec2{0.0f};
    if (input.move_forward) {
      combined += glm::vec2{0.0f, 1.0f};
    }
    if (input.move_backward) {
      combined += glm::vec2{0.0f, -1.0f};
    }
    if (input.move_left) {
      combined += glm::vec2{-1.0f, 0.0f};
    }
    if (input.move_right) {
      combined += glm::vec2{1.0f, 0.0f};
    }
    glm::vec3 expected_position = glm::vec3{0.0f, 1.0f, 0.0f};
    if (glm::length(combined) > 0.0f) {
      combined = glm::normalize(combined);
      expected_position +=
          glm::vec3{combined.x * right + combined.y * forward} * 20.0f;
    }

    TestPlayerInput(input, expected_position);
  }
}
