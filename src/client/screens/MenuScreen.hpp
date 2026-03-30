#pragma once

#include <Utils/Logging/Logger.h>

#include <string>

#include "client/ProcessSpawner.hpp"
#include "client/network/ClientNetwork.hpp"
#include "client/screen/Screen.hpp"

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

class MenuScreen : public screen::Screen {
   public:
    MenuScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network, std::string mapPath,
               std::string serverAddress, int serverPort);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;

    [[nodiscard]] bool shouldQuit() const;

   private:
    void hostGame();
    void joinGame();

    screen::ScreenManager &m_screenManager;
    network::ClientNetwork &m_network;
    std::string m_mapPath;
    std::string m_serverAddress;
    int m_serverPort;

    ProcessSpawner m_spawner;
    bool m_shouldQuit = false;
    Utils::Logging::Logger m_logger{"MenuScreen"};
};

}  // namespace pacman::client::screens
