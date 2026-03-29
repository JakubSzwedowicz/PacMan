#include "client/graphics/Window.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::client::graphics {

Window::Window(const std::string &title, unsigned int width, unsigned int height)
    : m_window(sf::VideoMode({width, height}), title) {
    LOG_I("Window created: {}x{}", width, height);
}

void Window::pollEvents(const std::function<void(const sf::Event &)> &callback) {
    while (const auto event = m_window.pollEvent()) {
        callback(*event);
    }
}

sf::RenderWindow &Window::getRenderWindow() { return m_window; }

bool Window::isOpen() const { return m_window.isOpen(); }

void Window::clear() { m_window.clear(sf::Color::Black); }

void Window::display() { m_window.display(); }

}  // namespace pacman::client::graphics
