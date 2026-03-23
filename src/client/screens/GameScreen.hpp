#pragma once

#include "client/graphics/Renderer.hpp"
#include "client/screen/Screen.hpp"
#include "core/maps/Map.hpp"
#include "core/simulation/Simulation.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <entt/entt.hpp>

#include <memory>
#include <string>

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

class GameScreen : public screen::Screen {
public:
  GameScreen(screen::ScreenManager &screenManager,
             std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig,
             std::string mapPath);

  void onEnter() override;
  void onExit() override;
  void handleEvent(const sf::Event &event) override;
  void update(float dt) override;
  void draw(sf::RenderWindow &window) override;

  void setLastInput(const core::ecs::Input &input);

private:
  void spawnEntitiesFromMap();

  screen::ScreenManager &m_screenManager;
  std::shared_ptr<Utils::Logging::LoggerConfig> m_loggerConfig;
  std::string m_mapPath;

  entt::registry m_registry;
  core::maps::Map m_map;
  core::simulation::Simulation m_simulation;
  graphics::Renderer m_renderer;
  entt::entity m_localPlayer = entt::null;
  core::ecs::Input m_lastInput{0, core::ecs::Direction::None};

  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::screens
