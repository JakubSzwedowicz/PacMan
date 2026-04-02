#include "client/graphics/Renderer.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "core/ecs/Components.hpp"

namespace pacman::client::graphics {

namespace {

sf::Color ghostColor(const core::ecs::GhostState& ghostState) {
    using Mode = core::ecs::GhostState::Mode;
    using Type = core::ecs::GhostType;

    switch (ghostState.mode) {
        case Mode::Frightened:
            return sf::Color(40, 90, 255);
        case Mode::Eaten:
            return sf::Color(230, 230, 255);
        case Mode::InHouse:
        case Mode::Exiting:
        case Mode::Chase:
        case Mode::Scatter:
            break;
    }

    switch (ghostState.type) {
        case Type::Blinky:
            return sf::Color(255, 0, 0);
        case Type::Pinky:
            return sf::Color(255, 105, 180);
        case Type::Inky:
            return sf::Color(0, 255, 255);
        case Type::Clyde:
            return sf::Color(255, 165, 0);
    }

    return sf::Color::Red;
}

}  // namespace

Renderer::Renderer() { LOG_I("Renderer created"); }

void Renderer::render(sf::RenderWindow& window, const entt::registry& registry, const core::maps::Map& map) {
    float ts = map.tileSize;

    // Walls — drawn from the static map grid (never change)
    for (int row = 0; row < static_cast<int>(map.height); row++) {
        for (int col = 0; col < static_cast<int>(map.width); col++) {
            if (map.tileTypeAt(col, row) == core::maps::TileType::Wall) {
                sf::RectangleShape rect({ts, ts});
                rect.setPosition({static_cast<float>(col) * ts, static_cast<float>(row) * ts});
                rect.setFillColor(sf::Color(0, 0, 139));
                window.draw(rect);
            }
        }
    }

    // Pellets — drawn from ECS so they disappear when eaten
    auto pelletView = registry.view<const core::ecs::Position, const core::ecs::PelletTag>();
    for (auto entity : pelletView) {
        const auto& pos = pelletView.get<const core::ecs::Position>(entity);
        float radius = ts * 0.1f;
        sf::CircleShape dot(radius);
        dot.setPosition({pos.x + ts / 2.0f - radius, pos.y + ts / 2.0f - radius});
        dot.setFillColor(sf::Color::Yellow);
        window.draw(dot);
    }

    // Power pellets — drawn from ECS so they disappear when eaten
    auto powerPelletView = registry.view<const core::ecs::Position, const core::ecs::PowerPelletTag>();
    for (auto entity : powerPelletView) {
        const auto& pos = powerPelletView.get<const core::ecs::Position>(entity);
        float radius = ts * 0.25f;
        sf::CircleShape pellet(radius);
        pellet.setPosition({pos.x + ts / 2.0f - radius, pos.y + ts / 2.0f - radius});
        pellet.setFillColor(sf::Color::Yellow);
        window.draw(pellet);
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

    auto ghostView =
        registry.view<const core::ecs::Position, const core::ecs::Collider, const core::ecs::GhostState,
                      const core::ecs::GhostTag>();
    for (auto entity : ghostView) {
        const auto& pos = ghostView.get<const core::ecs::Position>(entity);
        const auto& col = ghostView.get<const core::ecs::Collider>(entity);
        const auto& ghostState = ghostView.get<const core::ecs::GhostState>(entity);
        float radius = col.width / 2.0f;
        sf::CircleShape shape(radius);
        shape.setPosition({pos.x, pos.y});
        shape.setFillColor(ghostColor(ghostState));
        window.draw(shape);
    }
}

}  // namespace pacman::client::graphics
