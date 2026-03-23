#include "client/ClientApp.hpp"
#include "client/screens/GameScreen.hpp"
#include "client/screens/MenuScreen.hpp"
#include "core/Common.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <SFML/System/Clock.hpp>

namespace pacman::client {

ClientApp::ClientApp(core::ClientConfig config,
                     std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_config(std::move(config)), m_loggerConfig(std::move(loggerConfig)),
      m_window("PacMan", static_cast<unsigned int>(m_config.windowWidth),
               static_cast<unsigned int>(m_config.windowHeight),
               m_loggerConfig),
      m_screenManager(m_loggerConfig), m_inputManager(m_loggerConfig),
      m_uiOverlay(m_loggerConfig), m_logger("ClientApp", m_loggerConfig) {
  LOG_I("ClientApp created");
}

void ClientApp::run() {
  LOG_I("ClientApp starting main loop");

  m_uiOverlay.init(m_window.getRenderWindow());

  m_screenManager.setScreen(std::make_unique<screens::MenuScreen>(
      m_screenManager, m_loggerConfig, m_config.mapPath));
  m_screenManager.applyPendingTransition();

  sf::Clock deltaClock;
  float accumulator = 0.0f;

  while (m_window.isOpen() && !m_shouldQuit) {
    sf::Time frameTime = deltaClock.restart();
    float dt = frameTime.asSeconds();
    if (dt > 0.1f) {
      dt = 0.1f;
    }
    accumulator += dt;

    m_window.pollEvents([this](const sf::Event &event) {
      if (event.is<sf::Event::Closed>()) {
        m_window.getRenderWindow().close();
        return;
      }

      m_uiOverlay.processEvent(m_window.getRenderWindow(), event);
      m_inputManager.handleEvent(event);
      m_screenManager.handleEvent(event);
    });

    auto input = m_inputManager.poll(m_tick);

    auto *gameScreen = dynamic_cast<screens::GameScreen *>(
        &*m_screenManager.getCurrentScreen());
    if (gameScreen) {
      gameScreen->setLastInput(input);
    }

    while (accumulator >= core::tickDt) {
      m_screenManager.update(core::tickDt);
      m_tick++;
      accumulator -= core::tickDt;
    }

    m_screenManager.applyPendingTransition();

    auto *menuScreen = dynamic_cast<screens::MenuScreen *>(
        &*m_screenManager.getCurrentScreen());
    if (menuScreen && menuScreen->shouldQuit()) {
      m_shouldQuit = true;
    }

    m_uiOverlay.update(m_window.getRenderWindow(), frameTime);

    m_window.clear();
    m_screenManager.draw(m_window.getRenderWindow());
    m_uiOverlay.render(m_window.getRenderWindow());
    m_window.display();
  }

  m_uiOverlay.shutdown();
  LOG_I("ClientApp main loop ended (tick={})", m_tick);
}

} // namespace pacman::client
