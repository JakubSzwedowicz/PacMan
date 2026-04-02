#pragma once

#include <Utils/Logging/Logger.h>

#include <entt/entt.hpp>

#include "core/maps/Map.hpp"

namespace pacman::server::phases {

struct RuleEvents {
    bool powerPelletEaten = false;
};

// Applies game rules each server tick: pellet and power-pellet pickup, ghost
// collisions, life loss. The registry is mutated in place (entities destroyed,
// scores updated). RuleEvents is returned so GamePhase can notify systems that
// need to react (e.g. AISystem::onPowerPelletEaten).
class AuthoritativeLogic {
   public:
    AuthoritativeLogic();

    RuleEvents applyRules(entt::registry &registry, const core::maps::Map &map, float dt);

   private:
    void updatePlayerTimers(entt::registry &registry, float dt);
    void checkPelletPickup(entt::registry &registry, const core::maps::Map &map);
    bool checkPowerPellet(entt::registry &registry, const core::maps::Map &map);
    void checkGhostCollision(entt::registry &registry, const core::maps::Map &map);

    Utils::Logging::Logger m_logger{"AuthoritativeLogic"};
};

}  // namespace pacman::server::phases
