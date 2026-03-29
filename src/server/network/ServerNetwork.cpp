#include "server/network/ServerNetwork.hpp"
#include "server/network/NetworkEventParser.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::network {

ServerNetwork::ServerNetwork() {
    auto enetSource = std::make_unique<core::network::ENetSourceProvider>();
    m_enetSource = enetSource.get(); // save raw ptr before ownership transfer

    m_eventProvider = std::make_unique<EventProvider>(
        std::move(enetSource),
        std::make_unique<NetworkEventParser>());

    LOG_I("ServerNetwork created");
}

ServerNetwork::~ServerNetwork() { stop(); }

bool ServerNetwork::start(uint16_t port, int maxClients) {
    LOG_I("Starting on port {} (maxClients={})", port, maxClients);
    return m_enetSource->startServer(port, maxClients);
}

void ServerNetwork::stop() {
    if (m_enetSource) m_enetSource->stop();
}

bool ServerNetwork::isRunning() const {
    return m_enetSource && m_enetSource->isActive();
}

void ServerNetwork::run() {
    m_eventProvider->run(); // ENetSourceProvider::run() + parse loop → fills queue
    while (auto event = m_eventProvider->poll())
        publish(*event);
}

// ---------------------------------------------------------------------------
// Outgoing — stubbed until Phase 4 wires ENetSourceProvider::sendTo/broadcast
// ---------------------------------------------------------------------------

void ServerNetwork::sendLobbyState(const core::protocol::LobbyStatePacket & /*p*/) {
    // TODO (Phase 4): serialize p to JSON, m_enetSource->sendTo(hostPeerId, ...)
}

void ServerNetwork::broadcastGameStart(
    const core::protocol::GameStartPacket & /*templatePkt*/,
    const std::array<core::PlayerId, core::maxPlayers> & /*playerIds*/,
    uint8_t /*count*/) {
    // TODO (Phase 4): per-peer copy with assignedPlayerId filled in
}

void ServerNetwork::broadcastSnapshot(const core::protocol::GameSnapshotPacket & /*p*/) {}
void ServerNetwork::broadcastRoundEnd(const core::protocol::RoundEndPacket & /*p*/) {}
void ServerNetwork::broadcastShutdown(const core::protocol::ServerShutdownPacket & /*p*/) {}

} // namespace pacman::server::network
