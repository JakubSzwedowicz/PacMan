#pragma once

#include <Utils/Logging/LoggerSubscribed.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace pacman::client::ui {

class UIOverlay {
public:
  UIOverlay();
  ~UIOverlay();

  UIOverlay(const UIOverlay &) = delete;
  UIOverlay &operator=(const UIOverlay &) = delete;
  UIOverlay(UIOverlay &&) = delete;
  UIOverlay &operator=(UIOverlay &&) = delete;

  void init(sf::RenderWindow &window);
  void processEvent(const sf::RenderWindow &window, const sf::Event &event);
  void update(sf::RenderWindow &window, sf::Time dt);
  void render(sf::RenderWindow &window);
  void shutdown();

private:
  bool m_initialized = false;
  Utils::Logging::LoggerSubscribed m_logger{"UIOverlay"};
};

} // namespace pacman::client::ui
