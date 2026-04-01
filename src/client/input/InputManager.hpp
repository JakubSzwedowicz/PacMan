#pragma once

#include <Utils/Logging/Logger.h>

#include <SFML/Window/Event.hpp>

#include "client/input/InputSnapshot.hpp"

namespace pacman::client::input {

class InputMapper {
   public:
    InputMapper();

    void feed(const sf::Event &event);
    InputSnapshot snapshot(core::Tick tick);

   private:
    core::ecs::Direction m_currentDirection = core::ecs::Direction::None;
    bool m_escapePressed = false;
    Utils::Logging::Logger m_logger{"InputMapper"};
};

}  // namespace pacman::client::input
