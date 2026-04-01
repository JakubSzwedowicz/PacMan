#pragma once

#include <Utils/Logging/Logger.h>

#include "client/app/ProcessSpawner.hpp"
#include "client/network/ClientNetwork.hpp"
#include "client/ScreenManagement/Screen.hpp"

namespace pacman::client::screens {

class MenuScreen : public screen::Screen {
   public:
    MenuScreen(network::ClientNetwork &network, ProcessSpawner &spawner);

    void onEnter() override;
    void onExit() override;
    screen::ScreenRequest update(float dt, const input::InputSnapshot &input) override;
    void draw(sf::RenderWindow &window) override;

   private:
    void hostGame();
    void joinGame();

    network::ClientNetwork &m_network;
    ProcessSpawner &m_spawner;
    Utils::Logging::Logger m_logger{"MenuScreen"};
};

}  // namespace pacman::client::screens
