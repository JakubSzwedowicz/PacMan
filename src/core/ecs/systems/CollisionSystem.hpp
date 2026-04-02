#pragma once

#include <entt/entt.hpp>

#include "core/maps/Map.hpp"

namespace pacman::core::ecs::systems {

void resolveWallCollisions(entt::registry &registry, const maps::Map &map);
void resolveWallCollisions(entt::registry &registry, entt::entity entity, const maps::Map &map);

}  // namespace pacman::core::ecs::systems
