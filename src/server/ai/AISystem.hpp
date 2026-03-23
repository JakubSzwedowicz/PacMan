#pragma once

#include "server/ai/GhostPhaseTimer.hpp"

#include "core/maps/Map.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <entt/entt.hpp>

#include <memory>

namespace pacman::server::ai {

// Orchestrates all ghost AI each server tick.
// Delegates phase timing to GhostPhaseTimer and target calculation to
// GhostBehavior. Reads Position/DirectionState/GhostTag/GhostState components
// and writes back DirectionState::next for each ghost.
class AISystem {
public:
  explicit AISystem(
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  // Call once per server tick after MovementSystem has run.
  void update(entt::registry &registry, const core::maps::Map &map, float dt);

  // Called by GamePhase when any player eats a power-pellet.
  void onPowerPelletEaten();

private:
  GhostPhaseTimer m_phaseTimer;
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::server::ai
