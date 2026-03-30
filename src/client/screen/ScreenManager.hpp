#pragma once

#include <Utils/Logging/Logger.h>

#include <memory>

#include "client/screen/Screen.hpp"

namespace pacman::client::screen {

class ScreenManager {
   public:
    ScreenManager();

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
    Utils::Logging::Logger m_logger{"ScreenManager"};
};

}  // namespace pacman::client::screen
