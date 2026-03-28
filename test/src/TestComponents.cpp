#include "core/ecs/Components.hpp"

#include <entt/entt.hpp>
#include <gtest/gtest.h>

using namespace pacman::core;
using namespace pacman::core::ecs;

class ComponentsFixture : public ::testing::Test {
protected:
  entt::registry registry;

  entt::entity spawnPacMan(float x, float y) {
    auto entity = registry.create();
    registry.emplace<Position>(entity, x, y);
    registry.emplace<Velocity>(entity, defaultSpeed);
    registry.emplace<DirectionState>(entity);
    registry.emplace<Collider>(entity, 14.0f, 14.0f);
    registry.emplace<PlayerState>(entity);
    registry.emplace<PacManTag>(entity);
    return entity;
  }

  entt::entity spawnGhost(float x, float y) {
    auto entity = registry.create();
    registry.emplace<Position>(entity, x, y);
    registry.emplace<Velocity>(entity, defaultSpeed);
    registry.emplace<DirectionState>(entity);
    registry.emplace<Collider>(entity, 14.0f, 14.0f);
    registry.emplace<GhostState>(entity);
    registry.emplace<GhostTag>(entity);
    return entity;
  }

  entt::entity spawnWall(float x, float y) {
    auto entity = registry.create();
    registry.emplace<Position>(entity, x, y);
    registry.emplace<Collider>(entity, 16.0f, 16.0f);
    registry.emplace<WallTag>(entity);
    return entity;
  }
};

TEST_F(ComponentsFixture, CreateAndDestroyEntities) {
  auto pacman = spawnPacMan(10.0f, 20.0f);
  auto ghost = spawnGhost(30.0f, 40.0f);

  EXPECT_TRUE(registry.valid(pacman));
  EXPECT_TRUE(registry.valid(ghost));

  registry.destroy(pacman);
  EXPECT_FALSE(registry.valid(pacman));
  EXPECT_TRUE(registry.valid(ghost));
}

TEST_F(ComponentsFixture, MarkerTagFiltering) {
  spawnPacMan(0, 0);
  spawnGhost(10, 10);
  spawnGhost(20, 20);
  spawnWall(30, 30);
  spawnWall(40, 40);
  spawnWall(50, 50);

  auto pacmanView = registry.view<PacManTag>();
  auto ghostView = registry.view<GhostTag>();
  auto wallView = registry.view<WallTag>();

  int pacmanCount = 0;
  for ([[maybe_unused]] auto e : pacmanView)
    pacmanCount++;
  EXPECT_EQ(pacmanCount, 1);

  int ghostCount = 0;
  for ([[maybe_unused]] auto e : ghostView)
    ghostCount++;
  EXPECT_EQ(ghostCount, 2);

  int wallCount = 0;
  for ([[maybe_unused]] auto e : wallView)
    wallCount++;
  EXPECT_EQ(wallCount, 3);
}

TEST_F(ComponentsFixture, ViewIteration) {
  auto e1 = spawnPacMan(5.0f, 10.0f);
  auto e2 = spawnGhost(15.0f, 20.0f);

  auto view = registry.view<const Position, const Velocity>();
  int count = 0;
  for (auto entity : view) {
    const auto &pos = view.get<const Position>(entity);
    const auto &vel = view.get<const Velocity>(entity);
    EXPECT_GT(vel.speed, 0.0f);
    if (entity == e1) {
      EXPECT_FLOAT_EQ(pos.x, 5.0f);
    } else if (entity == e2) {
      EXPECT_FLOAT_EQ(pos.x, 15.0f);
    }
    count++;
  }
  EXPECT_EQ(count, 2);
}

TEST_F(ComponentsFixture, DirectionStateDefaults) {
  auto entity = spawnPacMan(0, 0);
  auto &dir = registry.get<DirectionState>(entity);
  EXPECT_EQ(dir.current, Direction::None);
  EXPECT_EQ(dir.next, Direction::None);
}

TEST_F(ComponentsFixture, PlayerStateDefaults) {
  auto entity = spawnPacMan(0, 0);
  auto &ps = registry.get<PlayerState>(entity);
  EXPECT_EQ(ps.score, 0);
  EXPECT_EQ(ps.lives, 3);
  EXPECT_FALSE(ps.isPowered);
}
