#include "client/ClientApp.hpp"
#include "client/screens/GameScreen.hpp"
#include "client/screens/MenuScreen.hpp"

#include <Utils/Config/ConfigManagerWithLogger.h>
#include <Utils/Config/ConfigPublisher.h>
#include <Utils/Config/Providers/CLIConfigProvider.h>
#include <Utils/Config/Providers/JsonConfigProvider.h>
#include <Utils/Logging/LoggerMacros.h>

#include <SFML/System/Clock.hpp>

#include <fstream>
#include <string>
#include <string_view>

namespace pacman::client {

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int ClientApp::main(int argc, char *argv[]) {
  init(argc, argv);
  run();
  return 0;
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void ClientApp::init(int argc, char *argv[]) {
  using CLIProvider =
      Utils::Config::Providers::CLIConfigProvider<core::ClientConfig>;
  using JsonProvider =
      Utils::Config::Providers::JsonConfigProvider<core::ClientConfig>;

  Utils::Config::ConfigManagerWithLogger<core::ClientConfig, CLIProvider,
                                         JsonProvider>
      manager;

  std::string configPath = "config/client.json";
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::string_view(argv[i]) == "--config") {
      configPath = argv[i + 1];
      break;
    }
  }

  std::ifstream file(configPath);
  if (file.is_open()) {
    manager.update<JsonProvider>(file);
  }

  manager.update<CLIProvider>(argc, argv);

  m_config =
      static_cast<Utils::Config::ConfigPublisher<core::ClientConfig> &>(manager)
          .getConfig();

  // Window depends on config — constructed here, stored as optional member.
  m_window.emplace("PacMan",
                   static_cast<unsigned int>(m_config->windowWidth.get()),
                   static_cast<unsigned int>(m_config->windowHeight.get()));

  m_uiOverlay.init(m_window->getRenderWindow());

  m_screenManager.setScreen(std::make_unique<screens::MenuScreen>(
      m_screenManager, m_config->mapPath.get()));
  m_screenManager.applyPendingTransition();

  // Re-publish logger config to all LoggerSubscribed instances created so far.
  manager.resolve();

  LOG_I("ClientApp initialised");
}

// ---------------------------------------------------------------------------
// Event / render loop
// ---------------------------------------------------------------------------

void ClientApp::run() {
  LOG_I("ClientApp starting main loop");

  sf::Clock deltaClock;
  float accumulator = 0.0f;

  while (m_window->isOpen() && !m_shouldQuit) {
    sf::Time frameTime = deltaClock.restart();
    float dt = frameTime.asSeconds();
    if (dt > 0.1f) dt = 0.1f;
    accumulator += dt;

    m_window->pollEvents([this](const sf::Event &event) {
      if (event.is<sf::Event::Closed>()) {
        m_window->getRenderWindow().close();
        return;
      }
      m_uiOverlay.processEvent(m_window->getRenderWindow(), event);
      m_inputManager.handleEvent(event);
      m_screenManager.handleEvent(event);
    });

    auto input = m_inputManager.poll(m_tick);

    auto *gameScreen = dynamic_cast<screens::GameScreen *>(
        &*m_screenManager.getCurrentScreen());
    if (gameScreen) gameScreen->setLastInput(input);

    while (accumulator >= core::tickDt) {
      m_screenManager.update(core::tickDt);
      ++m_tick;
      accumulator -= core::tickDt;
    }

    m_screenManager.applyPendingTransition();

    auto *menuScreen = dynamic_cast<screens::MenuScreen *>(
        &*m_screenManager.getCurrentScreen());
    if (menuScreen && menuScreen->shouldQuit()) m_shouldQuit = true;

    m_uiOverlay.update(m_window->getRenderWindow(), frameTime);

    m_window->clear();
    m_screenManager.draw(m_window->getRenderWindow());
    m_uiOverlay.render(m_window->getRenderWindow());
    m_window->display();
  }

  m_uiOverlay.shutdown();
  LOG_I("ClientApp main loop ended (tick={})", m_tick);
}

} // namespace pacman::client
