#pragma once

#include <entt/entt.hpp>

namespace pacman::core::ecs::systems {

void updateMovement(entt::registry &registry, float dt);

}  // namespace pacman::core::ecs::systems
