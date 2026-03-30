#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <string>

#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "client/screen/Screen.hpp"
#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

// Shows the pre-game lobby: player list, ready toggle, Start button (host only).
//
// Transitions:
//   GameStartEvent received → LoadingScreen
//   ServerShutdownEvent / DisconnectedEvent / Escape → MenuScreen
class LobbyScreen : public screen::Screen,
                    public Utils::PublishSubscribe::ISubscriber<network::events::ClientNetworkEvent> {
   public:
    LobbyScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network, std::string mapPath,
                std::string serverAddress, int serverPort, core::PlayerId localPlayerId, bool isHost);

    // Screen
    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;

    // ISubscriber<ClientNetworkEvent>
    void onUpdate(const network::events::ClientNetworkEvent &event) override;

   private:
    void goToMenu();

    screen::ScreenManager &m_screenManager;
    network::ClientNetwork &m_network;
    std::string m_mapPath;
    std::string m_serverAddress;
    int m_serverPort;
    [[maybe_unused]] core::PlayerId m_localPlayerId;
    bool m_isHost;

    core::protocol::LobbyStatePacket m_lobbyState{};
    bool m_localReady = false;

    Utils::Logging::Logger m_logger{"LobbyScreen"};
};

}  // namespace pacman::client::screens
