#pragma once

#include <cstdint>

namespace pacman {

// Overloaded-visitor helper for std::visit.
// Usage: std::visit(overloaded{ [](TypeA&){...}, [](TypeB&){...} }, variant);
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace pacman

namespace pacman::core {

using Tick = uint32_t;
using PlayerId = uint32_t;

inline constexpr float tickRate = 60.0f;
inline constexpr float tickDt = 1.0f / tickRate;
inline constexpr float defaultTileSize = 16.0f;
inline constexpr float defaultTilesPerSecond = 5.0f;
inline constexpr float defaultSpeed = defaultTileSize * defaultTilesPerSecond;
[[nodiscard]] inline constexpr float speedForTileSize(float tileSize) { return tileSize * defaultTilesPerSecond; }
inline constexpr int maxPlayers = 4;
inline constexpr int ghostCount = 4;

}  // namespace pacman::core
