#pragma once

#include "core/maps/Map.hpp"

#include <entt/entt.hpp>

namespace pacman::core::systems {

void resolveWallCollisions(entt::registry &registry, const Map &map);

} // namespace pacman::core::systems
