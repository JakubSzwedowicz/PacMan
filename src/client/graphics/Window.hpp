#pragma once

#include <Utils/Logging/Logger.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <functional>
#include <string>

namespace pacman::client::graphics {

class Window {
   public:
    Window(const std::string &title, unsigned int width, unsigned int height);

    void pollEvents(const std::function<void(const sf::Event &)> &callback);

    sf::RenderWindow &getRenderWindow();
    [[nodiscard]] bool isOpen() const;
    void clear();
    void display();

   private:
    sf::RenderWindow m_window;
    Utils::Logging::Logger m_logger{"Window"};
};

}  // namespace pacman::client::graphics
