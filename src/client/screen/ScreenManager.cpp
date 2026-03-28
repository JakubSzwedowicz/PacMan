#include "client/screen/ScreenManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::client::screen {

ScreenManager::ScreenManager() {
  LOG_I("ScreenManager created");
}

void ScreenManager::setScreen(std::unique_ptr<Screen> screen) {
  m_pendingScreen = std::move(screen);
}

void ScreenManager::applyPendingTransition() {
  if (!m_pendingScreen) {
    return;
  }

  if (m_currentScreen) {
    LOG_I("Exiting current screen");
    m_currentScreen->onExit();
  }

  m_currentScreen = std::move(m_pendingScreen);
  LOG_I("Entering new screen");
  m_currentScreen->onEnter();
}

void ScreenManager::handleEvent(const sf::Event &event) {
  if (m_currentScreen) {
    m_currentScreen->handleEvent(event);
  }
}

void ScreenManager::update(float dt) {
  if (m_currentScreen) {
    m_currentScreen->update(dt);
  }
}

void ScreenManager::draw(sf::RenderWindow &window) {
  if (m_currentScreen) {
    m_currentScreen->draw(window);
  }
}

bool ScreenManager::hasScreen() const { return m_currentScreen != nullptr; }

Screen *ScreenManager::getCurrentScreen() const {
  return m_currentScreen.get();
}

} // namespace pacman::client::screen
