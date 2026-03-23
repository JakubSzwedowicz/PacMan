#include "server/ai/AISystem.hpp"
#include "server/ai/GhostBehavior.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::ai {

AISystem::AISystem(std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_logger("AISystem", std::move(loggerConfig)) {
  LOG_I("AISystem created");
}

void AISystem::update(entt::registry &registry, const core::maps::Map &map,
                      float dt) {
  m_phaseTimer.update(dt);

  // TODO: iterate ghost entities, compute target tile per personality via
  //       GhostBehavior, call GhostBehavior::chooseDirection(), write back
  //       DirectionState::next
  (void)registry;
  (void)map;
}

void AISystem::onPowerPelletEaten() {
  m_phaseTimer.onPowerPelletEaten();
  LOG_I("Ghosts frightened for 10 s");
}

} // namespace pacman::server::ai
