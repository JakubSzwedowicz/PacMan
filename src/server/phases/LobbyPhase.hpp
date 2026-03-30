#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <array>
#include <optional>
#include <string>

#include "core/maps/Map.hpp"
#include "server/network/NetworkEvents.hpp"
#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"

namespace pacman::server::phases {

class LobbyPhase : public Phase, public Utils::PublishSubscribe::ISubscriber<network::events::ServerNetworkEvent> {
   public:
    LobbyPhase(network::ServerNetwork &network, std::string mapPath, int maxPlayers);

    // Phase
    void onEnter() override;
    void onExit() override;

    // Returns PhaseRunning until all players ready; then StartGameRequest.
    PhaseRequest update(float dt) override;

    // ISubscriber<ServerNetworkEvent>
    void onUpdate(const network::events::ServerNetworkEvent &event) override;

   private:
    void handleConnect(core::PlayerId id);
    void handleDisconnect(core::PlayerId id);
    void handleLobbyReady(core::PlayerId id, bool ready);

    void broadcastLobbyState();
    [[nodiscard]] bool allPlayersReady() const;
    void requestStartGame();  // sets m_pendingRequest

    network::ServerNetwork &m_network;
    std::string m_mapPath;
    int m_maxPlayers;

    std::optional<core::maps::Map> m_map;

    // Set by subscriber callbacks; drained by update().
    std::optional<PhaseRequest> m_pendingRequest;

    struct PlayerSlot {
        core::PlayerId id = 0;
        std::string name;
        bool connected = false;
        bool ready = false;
    };
    std::array<PlayerSlot, core::maxPlayers> m_slots{};
    uint8_t m_playerCount = 0;

    Utils::Logging::Logger m_logger{"LobbyPhase"};
};

}  // namespace pacman::server::phases
