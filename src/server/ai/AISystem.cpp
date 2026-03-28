#include "server/ai/AISystem.hpp"
#include "server/ai/GhostBehavior.hpp"

#include "core/ecs/Components.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::ai {

AISystem::AISystem() {
  LOG_I("AISystem created");
}

void AISystem::update(entt::registry &registry, const core::maps::Map &map,
                      float dt) {
  m_phaseTimer.update(dt);

  // Find Blinky's position once — Inky needs it for its target calculation.
  float blinkyX = 0.0f;
  float blinkyY = 0.0f;
  auto ghostView =
      registry.view<const core::ecs::Position, const core::ecs::GhostState,
                    const core::ecs::GhostTag>();
  for (auto e : ghostView) {
    const auto &gs = ghostView.get<const core::ecs::GhostState>(e);
    if (gs.type == core::ecs::GhostType::Blinky) {
      const auto &pos = ghostView.get<const core::ecs::Position>(e);
      blinkyX = pos.x;
      blinkyY = pos.y;
      break;
    }
  }

  // Update each ghost's direction.
  auto updateView =
      registry.view<const core::ecs::Position, core::ecs::DirectionState,
                    core::ecs::GhostState, const core::ecs::GhostTag>();

  for (auto e : updateView) {
    const auto &pos = updateView.get<const core::ecs::Position>(e);
    auto &dirState = updateView.get<core::ecs::DirectionState>(e);
    auto &ghostState = updateView.get<core::ecs::GhostState>(e);

    // Sync mode from phase timer (frightened overrides chase/scatter).
    if (m_phaseTimer.isFrightened()) {
      ghostState.mode = core::ecs::GhostState::Mode::Frightened;
    } else if (ghostState.mode != core::ecs::GhostState::Mode::Eaten) {
      ghostState.mode = m_phaseTimer.isScatter()
                            ? core::ecs::GhostState::Mode::Scatter
                            : core::ecs::GhostState::Mode::Chase;
    }

    if (ghostState.mode == core::ecs::GhostState::Mode::Eaten) {
      continue; // TODO: route eaten ghost back to spawn house
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

    dirState.next =
        GhostBehavior::chooseDirection(registry, map, pos.x, pos.y,
                                       dirState.current, target);
  }
}

void AISystem::onPowerPelletEaten() {
  m_phaseTimer.onPowerPelletEaten();
  LOG_I("Ghosts frightened for 10 s");
}

} // namespace pacman::server::ai
