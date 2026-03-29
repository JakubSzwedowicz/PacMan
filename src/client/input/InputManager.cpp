#include "client/input/InputManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <SFML/Window/Keyboard.hpp>

namespace pacman::client::input {

InputManager::InputManager() { LOG_I("InputManager created"); }

void InputManager::handleEvent(const sf::Event &event) {
    const auto *keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (!keyPressed) {
        return;
    }

    switch (keyPressed->code) {
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
        default:
            break;
    }
}

core::ecs::Input InputManager::poll(core::Tick tick) {
    core::ecs::Input input{tick, m_currentDirection};
    return input;
}

}  // namespace pacman::client::input
