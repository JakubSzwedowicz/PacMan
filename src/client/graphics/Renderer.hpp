#pragma once

#include "core/maps/Map.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <entt/entt.hpp>

#include <memory>

namespace pacman::client::graphics {

class Renderer {
public:
  explicit Renderer(
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  void render(sf::RenderWindow &window, const entt::registry &registry,
              const core::maps::Map &map);

private:
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::graphics
