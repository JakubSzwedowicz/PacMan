#pragma once

#include "core/ecs/Components.hpp"
#include "core/maps/Map.hpp"

#include <Utils/Logging/Logger.h>
#include <entt/entt.hpp>

namespace pacman::core::simulation {

class Simulation {
public:
  Simulation();

  void applyInput(entt::registry &registry, entt::entity entity,
                  const ecs::Input &input);
  void update(entt::registry &registry, float dt, const maps::Map &map);

private:
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::core::simulation
