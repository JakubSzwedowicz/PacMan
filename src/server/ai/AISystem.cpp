#include "server/ai/AISystem.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/ecs/Components.hpp"
#include "server/ai/GhostBehavior.hpp"

namespace pacman::server::ai {

AISystem::AISystem() { LOG_I("AISystem created"); }

void AISystem::update(entt::registry& registry, const core::maps::Map& map, float dt) {
    m_phaseTimer.update(dt);

    const float ts = map.tileSize;
    const bool hasGhostHouse = !(map.ghostHouseExit.col() == 0 && map.ghostHouseExit.row() == 0);
    const auto exitCol = static_cast<int32_t>(map.ghostHouseExit.col());
    const auto exitRow = static_cast<int32_t>(map.ghostHouseExit.row());

    // Find Blinky's position once — Inky needs it for its target calculation.
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

        // Use center-of-entity tile: junction fires when the ghost's centre
        // crosses a tile boundary, not its top-left corner.  This gives ~ts/2
        // hysteresis that prevents snap-back oscillation at tile edges.
        const auto col = static_cast<int32_t>((pos.x + ts * 0.5f) / ts);
        const auto row = static_cast<int32_t>((pos.y + ts * 0.5f) / ts);

        // Only re-evaluate direction when the ghost enters a new tile.
        if (col == ghostState.lastDecisionCol && row == ghostState.lastDecisionRow) continue;
        ghostState.lastDecisionCol = col;
        ghostState.lastDecisionRow = row;

        LOG_D("Ghost {} ({}) tile ({},{}) pos=({:.1f},{:.1f})", toString(ghostState.type),
              core::ecs::GhostState::toString(ghostState.mode), col, row, pos.x, pos.y);

        // --- InHouse: navigate toward the ghost door then exit ---
        if (ghostState.mode == core::ecs::GhostState::Mode::InHouse) {
            if (!hasGhostHouse) {
                ghostState.mode = core::ecs::GhostState::Mode::Scatter;
                // fall through to normal direction selection
            } else if (row <= exitRow) {
                // Reached or entered the door tile — begin exiting.
                ghostState.mode = core::ecs::GhostState::Mode::Exiting;
                dirState.next = core::ecs::Direction::Up;
                continue;
            } else {
                // Still inside: steer toward the ghost door tile.
                const core::maps::Tile exitTile{{static_cast<size_t>(exitCol), static_cast<size_t>(exitRow)}};
                dirState.next = GhostBehavior::chooseDirection(registry, map, pos.x, pos.y, dirState.current, exitTile);
                continue;
            }
        }

        // --- Exiting: keep going up until clear of the door row ---
        if (ghostState.mode == core::ecs::GhostState::Mode::Exiting) {
            if (row < exitRow) {
                // Past the door — join the normal phase cycle.
                ghostState.mode = core::ecs::GhostState::Mode::Scatter;
                // fall through to normal direction selection
            } else {
                dirState.next = core::ecs::Direction::Up;
                continue;
            }
        }

        // Sync mode from phase timer (frightened overrides chase/scatter).
        if (m_phaseTimer.isFrightened()) {
            ghostState.mode = core::ecs::GhostState::Mode::Frightened;
        } else if (ghostState.mode != core::ecs::GhostState::Mode::Eaten) {
            ghostState.mode =
                m_phaseTimer.isScatter() ? core::ecs::GhostState::Mode::Scatter : core::ecs::GhostState::Mode::Chase;
        }

        if (ghostState.mode == core::ecs::GhostState::Mode::Eaten) {
            continue;  // TODO: route eaten ghost back to spawn house
        }

        core::maps::Tile target;
        switch (ghostState.type) {
            case core::ecs::GhostType::Blinky:
                target = GhostBehavior::blinkyTarget(registry, map);
                break;
            case core::ecs::GhostType::Pinky:
                target = GhostBehavior::pinkyTarget(registry, map);
                break;
            case core::ecs::GhostType::Inky:
                target = GhostBehavior::inkyTarget(registry, map, blinkyX, blinkyY);
                break;
            case core::ecs::GhostType::Clyde:
                target = GhostBehavior::clydeTarget(registry, map, pos.x, pos.y);
                break;
        }

        dirState.next = GhostBehavior::chooseDirection(registry, map, pos.x, pos.y, dirState.current, target);
    }
}

void AISystem::onPowerPelletEaten() {
    m_phaseTimer.onPowerPelletEaten();
    LOG_I("Ghosts frightened for 10 s");
}

}  // namespace pacman::server::ai
