#include "core/simulation/Simulation.hpp"
#include "core/ecs/Components.hpp"
#include "core/ecs/systems/CollisionSystem.hpp"
#include "core/ecs/systems/MovementSystem.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::core::simulation {

Simulation::Simulation() {
  LOG_I("Simulation created");
}

void Simulation::applyInput(entt::registry &registry, entt::entity entity,
                            const ecs::Input &input) {
  if (!registry.valid(entity)) {
    return;
  }

  auto *dir = registry.try_get<ecs::DirectionState>(entity);
  if (dir) {
    dir->next = input.dir;
  }
}

void Simulation::update(entt::registry &registry, float dt,
                        const maps::Map &map) {
  ecs::systems::updateMovement(registry, dt);
  ecs::systems::resolveWallCollisions(registry, map);
}

} // namespace pacman::core::simulation
