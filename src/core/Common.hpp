#pragma once

#include <cstdint>

namespace pacman::core {

using Tick = uint32_t;
using PlayerId = uint32_t;

inline constexpr float tickRate = 60.0f;
inline constexpr float tickDt = 1.0f / tickRate;
inline constexpr float defaultTileSize = 16.0f;
inline constexpr float defaultSpeed = 80.0f;
inline constexpr int maxPlayers = 4;
inline constexpr int ghostCount = 4;

} // namespace pacman::core
