#pragma once

#include "core/maps/Map.hpp"

#include <entt/entt.hpp>

namespace pacman::core::ecs::systems {

void resolveWallCollisions(entt::registry &registry, const maps::Map &map);

} // namespace pacman::core::ecs::systems
