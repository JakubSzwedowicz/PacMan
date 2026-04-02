#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <string>

#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "client/ScreenManagement/Screen.hpp"
#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screens {

// Shows the pre-game lobby: player list, ready toggle, Start button (host only).
//
// Transitions:
//   GameStartEvent received → LoadingScreen
//   ServerShutdownEvent / DisconnectedEvent / Escape → MenuScreen
class LobbyScreen : public screen::Screen,
                    public Utils::PublishSubscribe::ISubscriber<network::events::ClientNetworkEvent> {
   public:
    using screen::Screen::onUpdate;

    LobbyScreen(network::ClientNetwork &network, core::PlayerId localPlayerId, bool isHost);

    // Screen
    void onEnter() override;
    void onExit() override;
    screen::ScreenRequest update(float dt, const input::InputSnapshot &input) override;
    void draw(sf::RenderWindow &window) override;

    // ISubscriber<ClientNetworkEvent>
    void onUpdate(const network::events::ClientNetworkEvent &event) override;

   private:
    network::ClientNetwork &m_network;
    [[maybe_unused]] core::PlayerId m_localPlayerId;
    bool m_isHost;
    bool m_hostDetermined = false;  // set once first LobbyStatePacket arrives

    core::protocol::LobbyStatePacket m_lobbyState{};
    bool m_localReady = false;

    Utils::Logging::Logger m_logger{"LobbyScreen"};
};

}  // namespace pacman::client::screens
