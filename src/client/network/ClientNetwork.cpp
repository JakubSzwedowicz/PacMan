#include "client/network/ClientNetwork.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/protocol/PacketCodec.hpp"

namespace pacman::client::network {

ClientNetwork::~ClientNetwork() { disconnect(); }

bool ClientNetwork::connect(const std::string &host, uint16_t port) {
    LOG_I("Connecting to {}:{}", host, port);
    return m_enetSource.connectToServer(host, port);
}

void ClientNetwork::disconnect() { m_enetSource.stop(); }

bool ClientNetwork::isConnected() const { return m_enetSource.isActive(); }

void ClientNetwork::run() {
    m_enetSource.run();
    while (auto raw = m_enetSource.poll()) publish(m_parser.parse(std::move(*raw)));
}

// ---------------------------------------------------------------------------
// Outgoing
// ---------------------------------------------------------------------------

void ClientNetwork::sendLobbyReady(bool ready) {
    core::protocol::LobbyReadyPacket pkt{0, ready};  // peerId filled by server from connection
    auto data = core::protocol::PacketCodec::serialize(pkt);
    m_enetSource.sendTo(1, {data.data(), data.size()}, true);  // peerId 1 = server
}

void ClientNetwork::sendReadyToPlay() {
    core::protocol::ReadyToPlayPacket pkt{0};
    auto data = core::protocol::PacketCodec::serialize(pkt);
    m_enetSource.sendTo(1, {data.data(), data.size()}, true);
}

void ClientNetwork::sendInput(const core::protocol::PlayerInputPacket &packet) {
    auto data = core::protocol::PacketCodec::serialize(packet);
    m_enetSource.sendTo(1, {data.data(), data.size()}, false);  // unreliable — inputs are time-sensitive
}

}  // namespace pacman::client::network
