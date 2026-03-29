#pragma once

#include <Utils/Logging/LoggerSubscribed.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <entt/entt.hpp>

#include "core/maps/Map.hpp"

namespace pacman::client::graphics {

class Renderer {
   public:
    Renderer();

    void render(sf::RenderWindow &window, const entt::registry &registry, const core::maps::Map &map);

   private:
    Utils::Logging::LoggerSubscribed m_logger{"Renderer"};
};

}  // namespace pacman::client::graphics
