#pragma once

#include "client/graphics/Window.hpp"
#include "client/input/InputManager.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/ui/UIOverlay.hpp"
#include "core/Common.hpp"
#include "core/Config.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <memory>

namespace pacman::client {

class ClientApp {
public:
  ClientApp(core::ClientConfig config,
            std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig);

  void run();

private:
  core::ClientConfig m_config;
  std::shared_ptr<Utils::Logging::LoggerConfig> m_loggerConfig;

  graphics::Window m_window;
  screen::ScreenManager m_screenManager;
  input::InputManager m_inputManager;
  ui::UIOverlay m_uiOverlay;

  core::Tick m_tick = 0;
  bool m_shouldQuit = false;

  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client
