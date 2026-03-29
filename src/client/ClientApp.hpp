#pragma once

#include "client/graphics/Window.hpp"
#include "client/input/InputManager.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/ui/UIOverlay.hpp"
#include "client/Config.hpp"
#include "core/Common.hpp"

#include <Utils/Logging/LoggerSubscribed.h>
#include <Utils/Runnables/IRunnable.h>

#include <memory>
#include <optional>

namespace pacman::client {

class ClientApp
    : public Utils::PublishSubscribe::ISubscriber<std::shared_ptr<const ClientConfig>> {
public:
  void onUpdate(const std::shared_ptr<const ClientConfig> &config) override {
    m_config = config;
  }
  // Full client lifecycle: init (config + window) → run.
  // Call from int main() — never call run() or init() directly.
  int main(int argc, char *argv[]);

private:
  // Parses args, resolves config, opens window, loads initial screen.
  void init(int argc, char *argv[]);

  // Event/render loop — blocks until the window is closed.
  void run();

  std::shared_ptr<const ClientConfig> m_config;
  std::unique_ptr<Utils::Runnables::IRunnable> m_configManager;

  // Window depends on config (dimensions) so it is constructed in init().
  // std::optional keeps it as a direct (non-heap) member of ClientApp.
  std::optional<graphics::Window> m_window;

  screen::ScreenManager m_screenManager;
  input::InputManager m_inputManager;
  ui::UIOverlay m_uiOverlay;

  core::Tick m_tick = 0;
  bool m_shouldQuit = false;

  Utils::Logging::LoggerSubscribed m_logger{"ClientApp"};
};

} // namespace pacman::client
