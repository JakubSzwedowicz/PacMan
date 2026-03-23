#include "server/ai/GhostBehavior.hpp"

namespace pacman::server::ai {

core::maps::Tile
GhostBehavior::nearestPacManTile(const entt::registry & /*registry*/) {
  return {}; // TODO: query PacManTag + Position, pick closest
}

core::ecs::Direction
GhostBehavior::chooseDirection(const entt::registry & /*registry*/,
                               const core::maps::Map & /*map*/,
                               float /*ghostX*/, float /*ghostY*/,
                               core::ecs::Direction /*currentDir*/,
                               const core::maps::Tile & /*targetTile*/) {
  return core::ecs::Direction::None; // TODO: BFS / greedy tile-distance
}

core::maps::Tile
GhostBehavior::blinkyTarget(const entt::registry &registry) {
  return nearestPacManTile(registry);
}

core::maps::Tile
GhostBehavior::pinkyTarget(const entt::registry &registry) {
  return nearestPacManTile(registry); // TODO: 4-tile lookahead
}

core::maps::Tile
GhostBehavior::inkyTarget(const entt::registry &registry,
                           float /*blinkyX*/, float /*blinkyY*/) {
  return nearestPacManTile(registry); // TODO: Blinky-vector calculation
}

core::maps::Tile
GhostBehavior::clydeTarget(const entt::registry &registry,
                            float /*clydeX*/, float /*clydeY*/) {
  return nearestPacManTile(registry); // TODO: distance-based switch
}

} // namespace pacman::server::ai
