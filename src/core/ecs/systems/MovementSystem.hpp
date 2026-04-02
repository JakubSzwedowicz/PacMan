#pragma once

#include <entt/entt.hpp>

#include "core/maps/Map.hpp"

namespace pacman::core::ecs::systems {

void updateMovement(entt::registry &registry, float dt, const maps::Map &map);
void updateMovement(entt::registry &registry, entt::entity entity, float dt, const maps::Map &map);

}  // namespace pacman::core::ecs::systems
