#include "core/ecs/systems/MovementSystem.hpp"

#include <cmath>

#include "core/ecs/Components.hpp"

namespace pacman::core::ecs::systems {

namespace {

void updateMovementForEntity(entt::registry &registry, entt::entity entity, float dt, float tileSize) {
    auto *pos = registry.try_get<ecs::Position>(entity);
    auto *vel = registry.try_get<const ecs::Velocity>(entity);
    auto *dir = registry.try_get<ecs::DirectionState>(entity);
    if (!pos || !vel || !dir) return;

    // Promote next→current and snap to tile grid on the perpendicular axis
    // when the direction changes (prevents floating-point drift accumulation).
    if (dir->next != Direction::None) {
        if (dir->next != dir->current) {
            if (dir->next == Direction::Up || dir->next == Direction::Down) {
                pos->x = std::floor(pos->x / tileSize + 0.5f) * tileSize;
            } else {
                pos->y = std::floor(pos->y / tileSize + 0.5f) * tileSize;
            }
        }
        dir->current = dir->next;
        dir->next = Direction::None;
    }

    float dx = 0.0f;
    float dy = 0.0f;
    switch (dir->current) {
        case Direction::Up:
            dy = -vel->speed * dt;
            break;
        case Direction::Down:
            dy = vel->speed * dt;
            break;
        case Direction::Left:
            dx = -vel->speed * dt;
            break;
        case Direction::Right:
            dx = vel->speed * dt;
            break;
        case Direction::None:
            break;
    }

    pos->x += dx;
    pos->y += dy;
}

}  // namespace

void updateMovement(entt::registry &registry, float dt, const maps::Map &map) {
    auto view = registry.view<ecs::Position, const ecs::Velocity, ecs::DirectionState>();
    for (auto entity : view) {
        updateMovementForEntity(registry, entity, dt, map.tileSize);
    }
}

void updateMovement(entt::registry &registry, entt::entity entity, float dt, const maps::Map &map) {
    if (!registry.valid(entity)) return;
    updateMovementForEntity(registry, entity, dt, map.tileSize);
}

}  // namespace pacman::core::ecs::systems
