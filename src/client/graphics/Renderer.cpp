#include "client/graphics/Renderer.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "core/ecs/Components.hpp"

namespace pacman::client::graphics {

Renderer::Renderer() { LOG_I("Renderer created"); }

void Renderer::render(sf::RenderWindow& window, const entt::registry& registry, const core::maps::Map& map) {
    float ts = map.tileSize;

    for (int row = 0; row < static_cast<int>(map.height); row++) {
        for (int col = 0; col < static_cast<int>(map.width); col++) {
            const auto tileType = map.tileTypeAt(col, row);
            if (tileType == core::maps::TileType::Wall) {
                sf::RectangleShape rect({ts, ts});
                rect.setPosition({static_cast<float>(col) * ts, static_cast<float>(row) * ts});
                rect.setFillColor(sf::Color(0, 0, 139));
                window.draw(rect);
            } else if (tileType == core::maps::TileType::Pellet) {
                float radius = ts * 0.1f;
                sf::CircleShape dot(radius);
                dot.setPosition({static_cast<float>(col) * ts + ts / 2.0f - radius,
                                 static_cast<float>(row) * ts + ts / 2.0f - radius});
                dot.setFillColor(sf::Color::Yellow);
                window.draw(dot);
            } else if (tileType == core::maps::TileType::PowerPellet) {
                float radius = ts * 0.25f;
                sf::CircleShape pellet(radius);
                pellet.setPosition({static_cast<float>(col) * ts + ts / 2.0f - radius,
                                    static_cast<float>(row) * ts + ts / 2.0f - radius});
                pellet.setFillColor(sf::Color::Yellow);
                window.draw(pellet);
            }
        }
    }

    auto pacmanView = registry.view<const core::ecs::Position, const core::ecs::Collider, const core::ecs::PacManTag>();
    for (auto entity : pacmanView) {
        const auto& pos = pacmanView.get<const core::ecs::Position>(entity);
        const auto& col = pacmanView.get<const core::ecs::Collider>(entity);
        float radius = col.width / 2.0f;
        sf::CircleShape shape(radius);
        shape.setPosition({pos.x, pos.y});
        shape.setFillColor(sf::Color::Yellow);
        window.draw(shape);
    }

    auto ghostView = registry.view<const core::ecs::Position, const core::ecs::Collider, const core::ecs::GhostTag>();
    for (auto entity : ghostView) {
        const auto& pos = ghostView.get<const core::ecs::Position>(entity);
        const auto& col = ghostView.get<const core::ecs::Collider>(entity);
        float radius = col.width / 2.0f;
        sf::CircleShape shape(radius);
        shape.setPosition({pos.x, pos.y});
        shape.setFillColor(sf::Color::Red);
        window.draw(shape);
    }
}

}  // namespace pacman::client::graphics
