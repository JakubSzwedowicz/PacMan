#pragma once

#include <Utils/Logging/Logger.h>

#include <string>

#include "client/network/ClientNetwork.hpp"
#include "client/screen/Screen.hpp"
#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

// Shows the end-of-round ranking table.
// Host sees [Next Round] (→ LobbyScreen); all players see [Back to Menu].
//
// This screen is fully static — no network events expected after RoundEnd.
// The connection stays alive so the host can restart from lobby.
class ResultsScreen : public screen::Screen {
   public:
    ResultsScreen(screen::ScreenManager &screenManager,
                  network::ClientNetwork *network,  // nullptr if offline
                  core::protocol::RoundEndPacket results, core::PlayerId localPlayerId, bool isHost,
                  std::string mapPath, std::string serverAddress, int serverPort);

    // Screen
    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;

   private:
    void goToMenu();
    void goToLobby();

    screen::ScreenManager &m_screenManager;
    network::ClientNetwork *m_network;  // non-owning, may be nullptr
    core::protocol::RoundEndPacket m_results;
    core::PlayerId m_localPlayerId;
    bool m_isHost;
    std::string m_mapPath;
    std::string m_serverAddress;
    int m_serverPort;

    Utils::Logging::Logger m_logger{"ResultsScreen"};
};

}  // namespace pacman::client::screens
