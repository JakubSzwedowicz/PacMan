#pragma once

#include "core/ecs/Components.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <SFML/Window/Event.hpp>

#include <memory>

namespace pacman::client::input {

class InputManager {
public:
  explicit InputManager(
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  void handleEvent(const sf::Event &event);
  core::ecs::Input poll(core::Tick tick);

private:
  core::ecs::Direction m_currentDirection = core::ecs::Direction::None;
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::input
