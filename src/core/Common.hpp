#pragma once

#include <cstdint>

namespace pacman::core {

enum class Direction : uint8_t { Up, Down, Left, Right, None };

using Tick = uint32_t;
using PlayerId = uint32_t;

struct Input {
  Tick tick;
  Direction dir;
};

inline constexpr float tickRate = 60.0f;
inline constexpr float tickDt = 1.0f / tickRate;
inline constexpr float defaultTileSize = 16.0f;
inline constexpr float defaultSpeed = 80.0f;
inline constexpr int maxPlayers = 4;

} // namespace pacman::core
