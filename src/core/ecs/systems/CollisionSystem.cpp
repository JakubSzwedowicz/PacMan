#include "core/ecs/systems/CollisionSystem.hpp"

#include <algorithm>

#include "core/ecs/Components.hpp"
#include "core/maps/Map.hpp"

namespace pacman::core::ecs::systems {

void resolveWallCollisions(entt::registry &registry, const maps::Map &map) {
    auto view = registry.view<ecs::Position, const ecs::Collider>();

    float ts = map.tileSize;

    for (auto entity : view) {
        auto &pos = view.get<ecs::Position>(entity);
        const auto &col = view.get<const ecs::Collider>(entity);
        const bool isGhost = registry.any_of<ecs::GhostTag>(entity);

        float left = pos.x;
        float top = pos.y;
        float right = pos.x + col.width;
        float bottom = pos.y + col.height;

        int colMin = std::max(0, static_cast<int>(left / ts));
        int colMax = std::min(static_cast<int>(map.width) - 1, static_cast<int>(right / ts));
        int rowMin = std::max(0, static_cast<int>(top / ts));
        int rowMax = std::min(static_cast<int>(map.height) - 1, static_cast<int>(bottom / ts));

        for (int row = rowMin; row <= rowMax; ++row) {
            for (int c = colMin; c <= colMax; ++c) {
                const auto tt = map.tileTypeAt(static_cast<maps::Tile::Unit>(c), static_cast<maps::Tile::Unit>(row));
                const bool isWall = (tt == maps::TileType::Wall) || (tt == maps::TileType::GhostDoor && !isGhost);
                if (!isWall) {
                    continue;
                }

                float wallLeft = static_cast<float>(c) * ts;
                float wallTop = static_cast<float>(row) * ts;
                float wallRight = wallLeft + ts;
                float wallBottom = wallTop + ts;

                bool overlaps = pos.x < wallRight && pos.x + col.width > wallLeft && pos.y < wallBottom &&
                                pos.y + col.height > wallTop;

                if (!overlaps) {
                    continue;
                }

                float overlapLeft = pos.x + col.width - wallLeft;
                float overlapRight = wallRight - pos.x;
                float overlapTop = pos.y + col.height - wallTop;
                float overlapBottom = wallBottom - pos.y;

                float minOverlapX = std::min(overlapLeft, overlapRight);
                float minOverlapY = std::min(overlapTop, overlapBottom);

                if (minOverlapX < minOverlapY) {
                    if (overlapLeft < overlapRight) {
                        pos.x = wallLeft - col.width;
                    } else {
                        pos.x = wallRight;
                    }
                } else {
                    if (overlapTop < overlapBottom) {
                        pos.y = wallTop - col.height;
                    } else {
                        pos.y = wallBottom;
                    }
                }
            }
        }
    }
}

}  // namespace pacman::core::ecs::systems
