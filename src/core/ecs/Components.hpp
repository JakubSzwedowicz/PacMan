#pragma once

#include <cstdint>

#include "core/Common.hpp"
#include "core/maps/Map.hpp"

namespace pacman::core::ecs {

enum class Direction : uint8_t { Up, Down, Left, Right, None };

enum class GhostType : uint8_t { Blinky = 0, Pinky = 1, Inky = 2, Clyde = 3 };

[[nodiscard]] inline const char* toString(GhostType t) noexcept {
    switch (t) {
        case GhostType::Blinky:
            return "Blinky";
        case GhostType::Pinky:
            return "Pinky";
        case GhostType::Inky:
            return "Inky";
        case GhostType::Clyde:
            return "Clyde";
    }
    return "Unknown";
}

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
    int32_t lastTileCol = -1;
    int32_t lastTileRow = -1;
};

struct GhostState {
    enum class Mode : uint8_t { Chase, Scatter, Frightened, Eaten, InHouse, Exiting };

    [[nodiscard]] static const char* toString(Mode m) noexcept {
        switch (m) {
            case Mode::Chase:
                return "Chase";
            case Mode::Scatter:
                return "Scatter";
            case Mode::Frightened:
                return "Frightened";
            case Mode::Eaten:
                return "Eaten";
            case Mode::InHouse:
                return "InHouse";
            case Mode::Exiting:
                return "Exiting";
        }
        return "Unknown";
    }
    Mode mode = Mode::InHouse;
    GhostType type = GhostType::Blinky;
    // Persistent target tile for current phase (Chase/Scatter/InHouse).
    // Updated only on phase transitions, not every frame.
    // For Frightened mode, this is unused (random direction instead).
    maps::Tile targetTile = {};
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
