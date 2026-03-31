#pragma once

#include <cstdint>

#include "core/Common.hpp"

namespace pacman::core::ecs {

enum class Direction : uint8_t { Up, Down, Left, Right, None };

enum class GhostType : uint8_t { Blinky = 0, Pinky = 1, Inky = 2, Clyde = 3 };

struct Input {
    core::Tick tick;
    Direction dir;
};

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
    enum class Mode : uint8_t { Chase, Scatter, Frightened, Eaten, InHouse, Exiting };
    Mode mode = Mode::InHouse;
    GhostType type = GhostType::Blinky;
    // Tile where the last direction decision was made; -1 = no decision yet.
    int32_t lastDecisionCol = -1;
    int32_t lastDecisionRow = -1;
};

struct PacManTag {};
struct GhostTag {};
struct PelletTag {};
struct PowerPelletTag {};
struct WallTag {};

}  // namespace pacman::core::ecs
