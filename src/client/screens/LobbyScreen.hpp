#pragma once

#include <Utils/Logging/LoggerSubscribed.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "client/screen/Screen.hpp"
#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

// Shows the pre-game lobby: player list, ready toggle, Start button (host
// only).
//
// Lifecycle:
//   onEnter → RAII subscription to ClientNetwork events begins at construction
//   onExit  → subscription ends at destruction
//
// Transitions:
//   GameStartEvent received → setScreen<LoadingScreen>
//   ServerShutdownEvent / DisconnectedEvent → setScreen<MenuScreen>
class LobbyScreen : public screen::Screen,
                    public Utils::PublishSubscribe::ISubscriber<network::events::ClientNetworkEvent> {
   public:
    LobbyScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network, core::PlayerId localPlayerId,
                bool isHost);

    // Screen
    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;

    // ISubscriber<ClientNetworkEvent>
    void onUpdate(const network::events::ClientNetworkEvent &event) override;

   private:
    screen::ScreenManager &m_screenManager;
    network::ClientNetwork &m_network;
    core::PlayerId m_localPlayerId;
    bool m_isHost;

    core::protocol::LobbyStatePacket m_lobbyState{};
    bool m_localReady = false;

    Utils::Logging::LoggerSubscribed m_logger{"LobbyScreen"};
};

}  // namespace pacman::client::screens
