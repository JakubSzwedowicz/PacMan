#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <functional>
#include <memory>
#include <string>

namespace pacman::client::graphics {

class Window {
public:
  Window(const std::string &title, unsigned int width, unsigned int height,
         std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  void pollEvents(const std::function<void(const sf::Event &)> &callback);

  sf::RenderWindow &getRenderWindow();
  [[nodiscard]] bool isOpen() const;
  void clear();
  void display();

private:
  sf::RenderWindow m_window;
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::graphics
