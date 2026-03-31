#include "core/ecs/systems/MovementSystem.hpp"

#include <cmath>

#include "core/ecs/Components.hpp"

namespace pacman::core::ecs::systems {

void updateMovement(entt::registry &registry, float dt) {
    auto view = registry.view<ecs::Position, const ecs::Velocity, ecs::DirectionState>();

    for (auto entity : view) {
        auto &pos = view.get<ecs::Position>(entity);
        const auto &vel = view.get<const ecs::Velocity>(entity);
        auto &dir = view.get<ecs::DirectionState>(entity);

        // Promote next→current and snap to tile grid on the perpendicular axis
        // when the direction changes (prevents floating-point drift accumulation).
        if (dir.next != Direction::None) {
            if (dir.next != dir.current) {
                // Snap the axis we're NOT moving along so it's aligned to the tile
                // whose centre the entity is currently nearest.  Using floor(pos/ts + 0.5)
                // (= centre-of-entity tile) instead of round(pos/ts) prevents the
                // snap from placing the entity back in the previous tile when it has
                // just crossed a boundary by a sub-pixel distance.
                constexpr float kTileSize = 32.0f;
                if (dir.next == Direction::Up || dir.next == Direction::Down) {
                    pos.x = std::floor(pos.x / kTileSize + 0.5f) * kTileSize;
                } else {
                    pos.y = std::floor(pos.y / kTileSize + 0.5f) * kTileSize;
                }
            }
            dir.current = dir.next;
            dir.next = Direction::None;  // Clear so alignment guard fires once per tile
        }

        float dx = 0.0f;
        float dy = 0.0f;
        switch (dir.current) {
            case Direction::Up:
                dy = -vel.speed * dt;
                break;
            case Direction::Down:
                dy = vel.speed * dt;
                break;
            case Direction::Left:
                dx = -vel.speed * dt;
                break;
            case Direction::Right:
                dx = vel.speed * dt;
                break;
            case Direction::None:
                break;
        }

        pos.x += dx;
        pos.y += dy;
    }
}

}  // namespace pacman::core::ecs::systems
