#include "client/ScreenManagement/ScreenManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::client::screen {

ScreenManager::ScreenManager(ScreenFactory factory) : m_factory(std::move(factory)) { LOG_I("ScreenManager created"); }

void ScreenManager::requestScreen(ScreenRequest request) { handleRequest(std::move(request)); }

void ScreenManager::handleRequest(ScreenRequest request) {
    if (std::holds_alternative<NoScreenRequest>(request)) {
        return;
    }

    if (std::holds_alternative<QuitAppRequest>(request)) {
        m_shouldQuit = true;
        return;
    }

    auto nextScreen = m_factory(std::move(request));
    if (!nextScreen) {
        LOG_E("ScreenFactory returned nullptr");
        return;
    }

    if (m_currentScreen) {
        LOG_I("Exiting current screen");
        m_currentScreen->onExit();
    }

    m_currentScreen = std::move(nextScreen);
    LOG_I("Entering new screen");
    m_currentScreen->onEnter();
}

void ScreenManager::update(float dt, const input::InputSnapshot& input) {
    if (m_currentScreen) {
        handleRequest(m_currentScreen->update(dt, input));
    }
}

void ScreenManager::draw(sf::RenderWindow &window) {
    if (m_currentScreen) {
        m_currentScreen->draw(window);
    }
}

bool ScreenManager::hasScreen() const { return m_currentScreen != nullptr; }

Screen *ScreenManager::getCurrentScreen() const { return m_currentScreen.get(); }

bool ScreenManager::shouldQuit() const { return m_shouldQuit; }

}  // namespace pacman::client::screen
