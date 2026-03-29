#include "client/ClientApp.hpp"
#include "client/screens/GameScreen.hpp"
#include "client/screens/MenuScreen.hpp"

#include <Utils/Config/ConfigManagerWithLogger.h>
#include <Utils/Config/Providers/CLIConfigProvider.h>
#include <Utils/Config/Providers/JsonConfigProvider.h>
#include <Utils/Logging/LoggerMacros.h>
#include <Utils/Providers/FileSourceProvider.h>

#include <SFML/System/Clock.hpp>

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
      Utils::Config::Providers::CLIConfigProvider<ClientConfig>;
  using JsonProvider =
      Utils::Config::Providers::JsonConfigProvider<ClientConfig>;
  using Manager =
      Utils::Config::ConfigManagerWithLogger<ClientConfig, CLIProvider,
                                             JsonProvider>;

  auto fileSource = std::make_unique<Utils::Providers::FileSourceProvider>(
      "config/client.json");
  auto *fileSourcePtr = fileSource.get();

  auto manager = std::make_unique<Manager>(
      std::make_unique<CLIProvider>(),
      std::make_unique<JsonProvider>(std::move(fileSource)));

  manager->update<CLIProvider>(argc, argv);

  fileSourcePtr->setPath(m_config->configPath.get());
  manager->run();

  m_configManager = std::move(manager);

  // Window depends on config — constructed here, stored as optional member.
  m_window.emplace("PacMan",
                   static_cast<unsigned int>(m_config->windowWidth.get()),
                   static_cast<unsigned int>(m_config->windowHeight.get()));

  m_uiOverlay.init(m_window->getRenderWindow());

  m_screenManager.setScreen(std::make_unique<screens::MenuScreen>(
      m_screenManager, m_config->mapPath.get()));
  m_screenManager.applyPendingTransition();

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

    m_configManager->run();
    m_network.run();

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
