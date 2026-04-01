#include "server/network/ServerNetwork.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/protocol/PacketCodec.hpp"

namespace pacman::server::network {

ServerNetwork::~ServerNetwork() { stop(); }

bool ServerNetwork::start(uint16_t port, int maxClients) {
    LOG_I("Starting on port {} (maxClients={})", port, maxClients);
    return m_enetSource.startServer(port, maxClients);
}

void ServerNetwork::stop() { m_enetSource.stop(); }

bool ServerNetwork::isRunning() const { return m_enetSource.isActive(); }

void ServerNetwork::run() {
    m_enetSource.run();
    while (auto raw = m_enetSource.poll()) publish(m_parser.parse(std::move(*raw)));
}

// ---------------------------------------------------------------------------
// Outgoing
// ---------------------------------------------------------------------------

void ServerNetwork::sendLobbyState(const core::protocol::LobbyStatePacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource.broadcast({data.data(), data.size()}, true);
}

void ServerNetwork::broadcastGameStart(core::protocol::GameStartPacket pkt) {
    // Send each peer a copy with their own assigned PlayerId filled in.
    // PlayerId == peerId in this implementation (see NetworkEventParser::parse).
    for (auto pid : pkt.playerIds) {
        pkt.assignedPlayerId = pid;
        auto data = core::protocol::PacketCodec::serialize(pkt);
        m_enetSource.sendTo(pid, {data.data(), data.size()}, true);
    }
}

void ServerNetwork::broadcastSnapshot(const core::protocol::GameSnapshotPacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource.broadcast({data.data(), data.size()}, false);
}

void ServerNetwork::broadcastRoundEnd(const core::protocol::RoundEndPacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource.broadcast({data.data(), data.size()}, true);
}

void ServerNetwork::broadcastShutdown(const core::protocol::ServerShutdownPacket &p) {
    auto data = core::protocol::PacketCodec::serialize(p);
    m_enetSource.broadcast({data.data(), data.size()}, true);
}

}  // namespace pacman::server::network
