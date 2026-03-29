#pragma once

#include <Utils/Logging/LoggerSubscribed.h>

#include <string>

#include "client/screen/Screen.hpp"

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

class MenuScreen : public screen::Screen {
   public:
    MenuScreen(screen::ScreenManager &screenManager, std::string mapPath);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;

    [[nodiscard]] bool shouldQuit() const;

   private:
    screen::ScreenManager &m_screenManager;
    std::string m_mapPath;
    bool m_shouldQuit = false;
    Utils::Logging::LoggerSubscribed m_logger{"MenuScreen"};
};

}  // namespace pacman::client::screens
