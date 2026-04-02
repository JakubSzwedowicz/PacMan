#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <entt/entt.hpp>
#include <optional>
#include <string>
#include <unordered_map>

#include "client/ScreenManagement/Screen.hpp"
#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "core/Common.hpp"
#include "core/maps/Map.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screens {

// Shown between LobbyScreen and GameScreen while the client loads assets,
// parses the map, initialises the local ECS, and waits for the server to
// confirm all players are ready.
//
// Lifecycle:
//   onEnter → RAII subscription to ClientNetwork events begins at construction
//   update  → advances through loading steps; sends ReadyToPlay when done
//   onExit  → subscription ends at destruction
//
// Transitions:
//   GameSnapshotEvent received → all players ready → setScreen<GameScreen>
//   ServerShutdownEvent / DisconnectedEvent → setScreen<MenuScreen>
class LoadingScreen : public screen::Screen,
                      public Utils::PublishSubscribe::ISubscriber<network::events::ClientNetworkEvent> {
   public:
    using screen::Screen::onUpdate;

    LoadingScreen(network::ClientNetwork& network, core::protocol::GameStartPacket gameStart,
                  core::PlayerId localPlayerId, bool isHost);

    // Screen
    void onEnter() override;
    void onExit() override;
    screen::ScreenRequest update(float dt, const input::InputSnapshot& input) override;
    void draw(sf::RenderWindow& window) override;

    // ISubscriber<ClientNetworkEvent>
    void onUpdate(const network::events::ClientNetworkEvent& event) override;

   private:
    void spawnEntitiesFromMap();

    network::ClientNetwork& m_network;
    core::protocol::GameStartPacket m_gameStart;
    core::PlayerId m_localPlayerId;
    bool m_isHost;

    core::maps::Map m_map;
    entt::registry m_registry;
    std::unordered_map<core::PlayerId, entt::entity> m_playerEntities;
    std::array<entt::entity, core::ghostCount> m_ghostEntities{};

    bool m_mapParsed = false;
    bool m_assetsLoaded = false;
    bool m_simInitialized = false;
    bool m_readyToPlaySent = false;
    bool m_firstSnapshotReceived = false;
    std::optional<core::protocol::GameSnapshotPacket> m_initialSnapshot;

    Utils::Logging::Logger m_logger{"LoadingScreen"};
};

}  // namespace pacman::client::screens
