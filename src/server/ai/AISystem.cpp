#include "server/ai/AISystem.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/ecs/Components.hpp"
#include "server/ai/GhostBehavior.hpp"

namespace pacman::server::ai {

AISystem::AISystem() { LOG_I("AISystem created"); }

void AISystem::update(entt::registry& registry, const core::maps::Map& map, float dt) {
    m_phaseTimer.update(dt);

    // Ghost AI architecture:
    // - Targets are PERSISTENT per phase (Chase/Scatter/InHouse), stored in GhostState::targetTile
    // - Direction decisions only happen at JUNCTIONS (tile entries), not every frame
    // - Special modes (InHouse/Exiting) have hardcoded escape logic
    // - Regular modes (Chase/Scatter/Frightened):
    //   * Chase/Scatter: greedy pathfinding toward persistent target
    //   * Frightened: random valid direction each junction
    //   * Eaten: TODO - route back to spawn house
    // - Mode changes (triggered by phase timer or events) update the persistent target
    // - Events that change targets: spawn, mode transition, power pellet eaten

    const float ts = map.tileSize;
    const bool hasGhostHouse = !(map.ghostHouseExit.col() == 0 && map.ghostHouseExit.row() == 0);
    const auto exitCol = static_cast<int32_t>(map.ghostHouseExit.col());
    const auto exitRow = static_cast<int32_t>(map.ghostHouseExit.row());

    // Find Blinky's position once — needed for Inky's target calculation.
    float blinkyX = 0.0f;
    float blinkyY = 0.0f;
    const auto blinkyView =
        registry.view<const core::ecs::Position, const core::ecs::GhostState, const core::ecs::GhostTag>();
    for (const auto e : blinkyView) {
        const auto& gs = blinkyView.get<const core::ecs::GhostState>(e);
        if (gs.type == core::ecs::GhostType::Blinky) {
            const auto& pos = blinkyView.get<const core::ecs::Position>(e);
            blinkyX = pos.x;
            blinkyY = pos.y;
            break;
        }
    }

    auto updateView = registry.view<const core::ecs::Position, core::ecs::DirectionState, core::ecs::GhostState,
                                    const core::ecs::GhostTag>();

    for (const auto e : updateView) {
        const auto& pos = updateView.get<const core::ecs::Position>(e);
        auto& dirState = updateView.get<core::ecs::DirectionState>(e);
        auto& ghostState = updateView.get<core::ecs::GhostState>(e);

        // Use center-of-entity tile: decisions fire when the ghost's centre
        // crosses a tile boundary. This gives ~ts/2 hysteresis.
        const auto col = static_cast<int32_t>((pos.x + ts * 0.5f) / ts);
        const auto row = static_cast<int32_t>((pos.y + ts * 0.5f) / ts);

        // Only evaluate direction at junctions (tile entries).
        if (col == ghostState.lastDecisionCol && row == ghostState.lastDecisionRow) continue;
        ghostState.lastDecisionCol = col;
        ghostState.lastDecisionRow = row;

        LOG_D("Ghost {} ({}) tile ({},{}) pos=({:.1f},{:.1f})", toString(ghostState.type),
              core::ecs::GhostState::toString(ghostState.mode), col, row, pos.x, pos.y);

        // --- InHouse: navigate toward ghost door, then transition to Exiting ---
        if (ghostState.mode == core::ecs::GhostState::Mode::InHouse) {
            if (!hasGhostHouse) {
                ghostState.mode = core::ecs::GhostState::Mode::Scatter;
                ghostState.targetTile = GhostBehavior::scatterTarget(ghostState.type, map);
                // Fall through to direction selection
            } else if (row <= exitRow) {
                // Reached the door — begin exiting (going up).
                ghostState.mode = core::ecs::GhostState::Mode::Exiting;
                ghostState.targetTile = core::maps::Tile{
                    {static_cast<core::maps::Tile::Unit>(exitCol),
                     exitRow > 0 ? static_cast<core::maps::Tile::Unit>(exitRow - 1) : 0}};
                dirState.next = core::ecs::Direction::Up;
                continue;
            } else {
                // Still inside: steer toward the exit.
                dirState.next = GhostBehavior::chooseDirection(registry, map, pos.x, pos.y, dirState.current,
                                                                map.ghostHouseExit);
                continue;
            }
        }

        // --- Exiting: keep going up until clear of the door row ---
        if (ghostState.mode == core::ecs::GhostState::Mode::Exiting) {
            if (row < exitRow) {
                // Past the door — transition to Chase/Scatter based on phase.
                ghostState.mode = m_phaseTimer.isScatter() ? core::ecs::GhostState::Mode::Scatter
                                                           : core::ecs::GhostState::Mode::Chase;
                ghostState.targetTile = GhostBehavior::selectTargetForMode(ghostState.mode, ghostState.type,
                                                                            registry, map, blinkyX, blinkyY);
                // Fall through to direction selection
            } else {
                dirState.next = core::ecs::Direction::Up;
                continue;
            }
        }

        // --- Eaten: route ghost back to spawn house ---
        if (ghostState.mode == core::ecs::GhostState::Mode::Eaten) {
            if (col == static_cast<int32_t>(ghostState.spawnTile.col()) &&
                row == static_cast<int32_t>(ghostState.spawnTile.row())) {
                // Reached spawn — transition back to InHouse.
                ghostState.mode = core::ecs::GhostState::Mode::InHouse;
                ghostState.targetTile = map.ghostHouseExit;
                LOG_D("Ghost {} (Eaten) returned to spawn, InHouse mode", toString(ghostState.type));
                // Fall through to direction selection
            } else {
                // Still routing home: pathfind toward spawn tile.
                dirState.next = GhostBehavior::chooseDirection(registry, map, pos.x, pos.y, dirState.current,
                                                                ghostState.spawnTile);
                continue;
            }
        }

        // --- Regular modes: Chase, Scatter, Frightened ---
        // Update mode based on phase timer, and update target if mode changed.
        auto oldMode = ghostState.mode;

        if (m_phaseTimer.isFrightened()) {
            ghostState.mode = core::ecs::GhostState::Mode::Frightened;
        } else if (ghostState.mode != core::ecs::GhostState::Mode::Eaten) {
            ghostState.mode = m_phaseTimer.isScatter() ? core::ecs::GhostState::Mode::Scatter
                                                       : core::ecs::GhostState::Mode::Chase;
        }

        // If mode changed, recalculate persistent target.
        if (ghostState.mode != oldMode && ghostState.mode != core::ecs::GhostState::Mode::Eaten) {
            ghostState.targetTile = GhostBehavior::selectTargetForMode(ghostState.mode, ghostState.type,
                                                                        registry, map, blinkyX, blinkyY);
        }

        // Pick direction based on current mode.
        if (ghostState.mode == core::ecs::GhostState::Mode::Frightened) {
            // Frightened: random valid direction each junction.
            dirState.next = GhostBehavior::chooseRandomDirection(map, pos.x, pos.y);
        } else if (ghostState.mode == core::ecs::GhostState::Mode::Eaten) {
            // Should not reach here (handled above), but skip if we do.
            continue;
        } else {
            // Chase or Scatter: use persistent target.
            dirState.next = GhostBehavior::chooseDirection(registry, map, pos.x, pos.y, dirState.current,
                                                            ghostState.targetTile);
        }
    }
}

void AISystem::onPowerPelletEaten() {
    m_phaseTimer.onPowerPelletEaten();
    LOG_I("Ghosts frightened for 10 s");
}

}  // namespace pacman::server::ai
