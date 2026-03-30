#include "server/network/ServerNetwork.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/protocol/PacketCodec.hpp"
#include "server/network/NetworkEventParser.hpp"

namespace pacman::server::network {

ServerNetwork::ServerNetwork() {
    auto enetSource = std::make_unique<core::network::ENetSourceProvider>();
    m_enetSource = enetSource.get();  // save raw ptr before ownership transfer

    m_eventProvider = std::make_unique<EventProvider>(std::move(enetSource), std::make_unique<NetworkEventParser>());

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

bool ServerNetwork::isRunning() const { return m_enetSource && m_enetSource->isActive(); }

void ServerNetwork::run() {
    m_eventProvider->run();  // ENetSourceProvider::run() + parse loop → fills queue
    while (auto event = m_eventProvider->poll()) publish(*event);
}

// ---------------------------------------------------------------------------
// Outgoing
// ---------------------------------------------------------------------------

void ServerNetwork::sendLobbyState(const core::protocol::LobbyStatePacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource->broadcast({data.data(), data.size()}, true);
}

void ServerNetwork::broadcastGameStart(core::protocol::GameStartPacket pkt) {
    // Send each peer a copy with their own assigned PlayerId filled in.
    // PlayerId == peerId in this implementation (see NetworkEventParser::parse).
    for (auto pid : pkt.playerIds) {
        pkt.assignedPlayerId = pid;
        auto data = core::protocol::PacketCodec::serialize(pkt);
        m_enetSource->sendTo(pid, {data.data(), data.size()}, true);
    }
}

void ServerNetwork::broadcastSnapshot(const core::protocol::GameSnapshotPacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource->broadcast({data.data(), data.size()}, false);
}

void ServerNetwork::broadcastRoundEnd(const core::protocol::RoundEndPacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource->broadcast({data.data(), data.size()}, true);
}

void ServerNetwork::broadcastShutdown(const core::protocol::ServerShutdownPacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource->broadcast({data.data(), data.size()}, true);
}

}  // namespace pacman::server::network
