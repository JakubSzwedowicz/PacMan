#pragma once

#include <entt/entt.hpp>

#include "core/ecs/Components.hpp"
#include "core/maps/Map.hpp"

namespace pacman::server::ai {

// Ghost AI behavior: target selection and direction pathfinding.
// Targets are persistent per phase (Chase/Scatter/InHouse); directions chosen only at junctions.
//
// Ghost personalities (Chase-mode targets):
//   Blinky — nearest PacMan tile (direct chase)
//   Pinky  — 4 tiles ahead of nearest PacMan's facing direction
//   Inky   — reflection of Blinky through a point 2 tiles ahead of PacMan
//   Clyde  — nearest PacMan when >8 tiles away; scatter corner otherwise
struct GhostBehavior {
    // Compute the target tile for a ghost based on its current mode and type.
    // Called when mode changes (not every frame).
    [[nodiscard]] static core::maps::Tile selectTargetForMode(
        core::ecs::GhostState::Mode mode, core::ecs::GhostType type,
        const entt::registry &registry, const core::maps::Map &map, float blinkyX = 0.0f, float blinkyY = 0.0f);

    // Pick a random valid direction (for Frightened mode).
    [[nodiscard]] static core::ecs::Direction chooseRandomDirection(const core::maps::Map &map, float ghostX,
                                                                    float ghostY);

    // Greedy tile-distance direction choice toward a target tile.
    // Prefers not reversing but allows it if no other option.
    // Returns Direction::None if all adjacent tiles are walls.
    [[nodiscard]] static core::ecs::Direction chooseDirection(const entt::registry &registry,
                                                              const core::maps::Map &map, float ghostX, float ghostY,
                                                              core::ecs::Direction currentDir,
                                                              const core::maps::Tile &targetTile);

    // Helper: returns first PacMan's tile, or {0,0} if none exists.
    [[nodiscard]] static core::maps::Tile nearestPacManTile(const entt::registry &registry, const core::maps::Map &map);

    // Target functions for each ghost (used by selectTargetForMode).
    [[nodiscard]] static core::maps::Tile blinkyTarget(const entt::registry &registry, const core::maps::Map &map);

    [[nodiscard]] static core::maps::Tile pinkyTarget(const entt::registry &registry, const core::maps::Map &map);

    [[nodiscard]] static core::maps::Tile inkyTarget(const entt::registry &registry, const core::maps::Map &map,
                                                     float blinkyX, float blinkyY);

    [[nodiscard]] static core::maps::Tile clydeTarget(const entt::registry &registry, const core::maps::Map &map,
                                                      float clydeX, float clydeY);

    // Scatter corners: northeast, northwest, southwest, southeast.
    [[nodiscard]] static core::maps::Tile scatterTarget(core::ecs::GhostType type, const core::maps::Map &map);
};

}  // namespace pacman::server::ai
