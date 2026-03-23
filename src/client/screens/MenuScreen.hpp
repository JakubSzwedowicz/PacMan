#pragma once

#include "client/screen/Screen.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <memory>

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

class MenuScreen : public screen::Screen {
public:
  MenuScreen(screen::ScreenManager &screenManager,
             std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig,
             std::string mapPath);

  void onEnter() override;
  void onExit() override;
  void handleEvent(const sf::Event &event) override;
  void update(float dt) override;
  void draw(sf::RenderWindow &window) override;

  [[nodiscard]] bool shouldQuit() const;

private:
  screen::ScreenManager &m_screenManager;
  std::shared_ptr<Utils::Logging::LoggerConfig> m_loggerConfig;
  std::string m_mapPath;
  bool m_shouldQuit = false;
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::screens
