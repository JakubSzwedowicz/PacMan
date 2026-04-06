#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"
#include "core/simulation/Simulation.hpp"

#include <entt/entt.hpp>
#include <gtest/gtest.h>

using namespace pacman::core;
using namespace pacman::core::maps;
using namespace pacman::core::ecs;
using namespace pacman::core::simulation;

class SimulationFixture : public ::testing::Test {
protected:
  entt::registry registry;
  Map map;
  Simulation simulation;
  entt::entity pacman = entt::null;

  void SetUp() override {
    auto result = MapsManager::loadFromJson(R"({
      "name": "Sim Test",
      "tileSize": 16.0,
      "tiles": ["###", "#.#", "###"],
      "pacmanSpawns": [{"pos": [1, 1]}],
      "ghostSpawns": {
        "blinky": {"pos": [0, 0]},
        "pinky": {"pos": [0, 0]},
        "inky": {"pos": [0, 0]},
        "clyde": {"pos": [0, 0]}
      }
    })");
    ASSERT_TRUE(result.has_value());
    map = std::move(*result);

    pacman = registry.create();
    registry.emplace<Position>(pacman, 16.0f, 16.0f);
    registry.emplace<Velocity>(pacman, 80.0f);
    registry.emplace<DirectionState>(pacman);
    registry.emplace<Collider>(pacman, 14.0f, 14.0f);
    registry.emplace<PacManTag>(pacman);
  }
};

TEST_F(SimulationFixture, ApplyInputChangesDirectionState) {
  Input input{0, Direction::Right};
  simulation.applyInput(registry, pacman, input);

  auto &dir = registry.get<DirectionState>(pacman);
  EXPECT_EQ(dir.next, Direction::Right);
}

TEST_F(SimulationFixture, UpdateMovesEntity) {
  Input input{0, Direction::Right};
  simulation.applyInput(registry, pacman, input);

  auto posBefore = registry.get<Position>(pacman);
  simulation.update(registry, tickDt, map);
  auto posAfter = registry.get<Position>(pacman);

  EXPECT_GT(posAfter.x, posBefore.x);
  EXPECT_FLOAT_EQ(posAfter.y, posBefore.y);
}

TEST_F(SimulationFixture, WallCollisionStopsMovement) {
  Input input{0, Direction::Up};
  simulation.applyInput(registry, pacman, input);

  for (int i = 0; i < 120; ++i) {
    simulation.update(registry, tickDt, map);
  }

  auto &pos = registry.get<Position>(pacman);
  EXPECT_GE(pos.y, 16.0f);
}

TEST_F(SimulationFixture, ApplyInputToInvalidEntity) {
  entt::entity invalid = entt::null;
  Input input{0, Direction::Left};
  EXPECT_NO_THROW(simulation.applyInput(registry, invalid, input));
}

TEST_F(SimulationFixture, NoMovementWithDirectionNone) {
  auto posBefore = registry.get<Position>(pacman);
  simulation.update(registry, tickDt, map);
  auto posAfter = registry.get<Position>(pacman);

  EXPECT_FLOAT_EQ(posAfter.x, posBefore.x);
  EXPECT_FLOAT_EQ(posAfter.y, posBefore.y);
}
