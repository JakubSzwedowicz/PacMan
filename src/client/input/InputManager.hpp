#pragma once

#include "core/ecs/Components.hpp"

#include <Utils/Logging/LoggerSubscribed.h>

#include <SFML/Window/Event.hpp>

namespace pacman::client::input {

class InputManager {
public:
  InputManager();

  void handleEvent(const sf::Event &event);
  core::ecs::Input poll(core::Tick tick);

private:
  core::ecs::Direction m_currentDirection = core::ecs::Direction::None;
  Utils::Logging::LoggerSubscribed m_logger{"InputManager"};
};

} // namespace pacman::client::input
