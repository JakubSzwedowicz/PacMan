#pragma once

#include "core/Common.hpp"

#include <cstdint>

namespace pacman::core::ecs {

struct Position {
  float x = 0.0f;
  float y = 0.0f;
};

struct Velocity {
  float speed = 0.0f;
};

struct DirectionState {
  Direction current = Direction::None;
  Direction next = Direction::None;
};

struct Collider {
  float width = 0.0f;
  float height = 0.0f;
};

struct PlayerState {
  int score = 0;
  int lives = 3;
  bool isPowered = false;
};

struct GhostState {
  enum class Mode : uint8_t { Chase, Scatter, Frightened, Eaten };
  Mode mode = Mode::Chase;
};

struct PacManTag {};
struct GhostTag {};
struct PelletTag {};
struct PowerPelletTag {};
struct WallTag {};

} // namespace pacman::core::ecs
