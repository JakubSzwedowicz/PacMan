#include "core/ecs/systems/MovementSystem.hpp"
#include "core/ecs/Components.hpp"

namespace pacman::core::ecs::systems {

void updateMovement(entt::registry &registry, float dt) {
  auto view =
      registry.view<ecs::Position, const ecs::Velocity, ecs::DirectionState>();

  for (auto entity : view) {
    auto &pos = view.get<ecs::Position>(entity);
    const auto &vel = view.get<const ecs::Velocity>(entity);
    auto &dir = view.get<ecs::DirectionState>(entity);

    if (dir.next != Direction::None) {
      dir.current = dir.next;
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

} // namespace pacman::core::ecs::systems
