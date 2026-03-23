#pragma once

#include "client/screen/Screen.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <memory>

namespace pacman::client::screen {

class ScreenManager {
public:
  explicit ScreenManager(
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  void setScreen(std::unique_ptr<Screen> screen);
  void applyPendingTransition();

  void handleEvent(const sf::Event &event);
  void update(float dt);
  void draw(sf::RenderWindow &window);

  [[nodiscard]] bool hasScreen() const;
  [[nodiscard]] Screen *getCurrentScreen() const;

private:
  std::unique_ptr<Screen> m_currentScreen;
  std::unique_ptr<Screen> m_pendingScreen;
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::screen
