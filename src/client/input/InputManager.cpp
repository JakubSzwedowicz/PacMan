#include "client/input/InputManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <SFML/Window/Keyboard.hpp>

namespace pacman::client::input {

InputMapper::InputMapper() { LOG_I("InputMapper created"); }

void InputMapper::feed(const sf::Event &event) {
    const auto *key = event.getIf<sf::Event::KeyPressed>();
    if (!key) return;

    switch (key->code) {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            m_currentDirection = core::ecs::Direction::Up;
            break;
        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            m_currentDirection = core::ecs::Direction::Down;
            break;
        case sf::Keyboard::Key::Left:
        case sf::Keyboard::Key::A:
            m_currentDirection = core::ecs::Direction::Left;
            break;
        case sf::Keyboard::Key::Right:
        case sf::Keyboard::Key::D:
            m_currentDirection = core::ecs::Direction::Right;
            break;
        case sf::Keyboard::Key::Escape:
            m_escapePressed = true;
            break;
        default:
            break;
    }
}

InputSnapshot InputMapper::snapshot(core::Tick tick) {
    InputSnapshot snap{m_currentDirection, tick, m_escapePressed};
    m_escapePressed = false;
    return snap;
}

}  // namespace pacman::client::input
