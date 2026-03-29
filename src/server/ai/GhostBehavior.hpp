#pragma once

#include <entt/entt.hpp>

#include "core/ecs/Components.hpp"
#include "core/maps/Map.hpp"

namespace pacman::server::ai {

// Pure stateless functions that compute the target tile and next direction for
// each ghost personality. All state (registry, map) is passed in; nothing is
// stored here.
//
// Ghost personalities (Chase-mode targets):
//   Blinky — nearest PacMan tile (direct chase)
//   Pinky  — 4 tiles ahead of nearest PacMan's facing direction
//   Inky   — reflection of Blinky through a point 2 tiles ahead of PacMan
//   Clyde  — nearest PacMan when >8 tiles away; scatter corner otherwise
struct GhostBehavior {
    // Returns the first PacMan's tile, or {0,0} if none exists.
    [[nodiscard]] static core::maps::Tile nearestPacManTile(const entt::registry &registry, const core::maps::Map &map);

    // Greedy tile-distance direction choice that never reverses.
    // Returns Direction::None if all adjacent tiles are walls.
    [[nodiscard]] static core::ecs::Direction chooseDirection(const entt::registry &registry,
                                                              const core::maps::Map &map, float ghostX, float ghostY,
                                                              core::ecs::Direction currentDir,
                                                              const core::maps::Tile &targetTile);

    [[nodiscard]] static core::maps::Tile blinkyTarget(const entt::registry &registry, const core::maps::Map &map);

    [[nodiscard]] static core::maps::Tile pinkyTarget(const entt::registry &registry, const core::maps::Map &map);

    // blinkyX/blinkyY — current pixel position of Blinky for the vector calculation.
    [[nodiscard]] static core::maps::Tile inkyTarget(const entt::registry &registry, const core::maps::Map &map,
                                                     float blinkyX, float blinkyY);

    // clydeX/clydeY — current pixel position of Clyde for the distance check.
    [[nodiscard]] static core::maps::Tile clydeTarget(const entt::registry &registry, const core::maps::Map &map,
                                                      float clydeX, float clydeY);
};

}  // namespace pacman::server::ai
