#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEventParser.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::client::network {

ClientNetwork::ClientNetwork() {
    auto enetSource = std::make_unique<core::network::ENetSourceProvider>();
    m_enetSource = enetSource.get(); // save raw ptr before ownership transfer

    m_eventProvider = std::make_unique<EventProvider>(
        std::move(enetSource),
        std::make_unique<ClientNetworkEventParser>());

    LOG_I("ClientNetwork created");
}

ClientNetwork::~ClientNetwork() { disconnect(); }

bool ClientNetwork::connect(const std::string &host, uint16_t port) {
    LOG_I("Connecting to {}:{}", host, port);
    return m_enetSource->connectToServer(host, port);
}

void ClientNetwork::disconnect() {
    if (m_enetSource) m_enetSource->stop();
}

bool ClientNetwork::isConnected() const {
    return m_enetSource && m_enetSource->isActive();
}

void ClientNetwork::run() {
    m_eventProvider->run(); // ENetSourceProvider::run() + parse loop → fills queue
    while (auto event = m_eventProvider->poll())
        publish(*event);
}

// ---------------------------------------------------------------------------
// Outgoing — stubbed until Phase 4 wires ENetSourceProvider::sendTo
// ---------------------------------------------------------------------------

void ClientNetwork::sendLobbyReady(bool /*ready*/) {
    // TODO (Phase 4): serialize LobbyReadyPacket via FlatBuffers + m_enetSource->sendTo(serverPeerId, ...)
}

void ClientNetwork::sendReadyToPlay() {
    // TODO (Phase 4): serialize ReadyToPlayPacket + m_enetSource->sendTo(serverPeerId, ...)
}

void ClientNetwork::sendInput(const core::protocol::PlayerInputPacket & /*packet*/) {
    // TODO (Phase 4): serialize PlayerInputPacket + m_enetSource->sendTo (unreliable channel)
}

} // namespace pacman::client::network
