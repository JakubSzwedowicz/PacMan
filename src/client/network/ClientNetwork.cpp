#include "client/network/ClientNetwork.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::client::network {

struct ClientNetwork::Impl {
  // TODO: ENet host + peer
};

ClientNetwork::ClientNetwork(
    std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_impl(std::make_unique<Impl>()),
      m_logger("ClientNetwork", std::move(loggerConfig)) {
  LOG_I("ClientNetwork created");
}

ClientNetwork::~ClientNetwork() { disconnect(); }

bool ClientNetwork::connect(const std::string &host, uint16_t port) {
  LOG_I("Connecting to {}:{}", host, port);
  // TODO: enet_host_create + enet_host_connect
  return false;
}

void ClientNetwork::disconnect() {
  // TODO: enet_peer_disconnect + enet_host_flush
}

bool ClientNetwork::isConnected() const {
  return false; // TODO
}

void ClientNetwork::setListener(IClientNetworkListener *listener) {
  m_listener = listener;
}

void ClientNetwork::sendLobbyReady(bool /*ready*/) {
  // TODO: serialize LobbyReadyPacket via FlatBuffers + enet_peer_send
}

void ClientNetwork::sendReadyToPlay() {
  // TODO: serialize ReadyToPlayPacket + enet_peer_send
}

void ClientNetwork::sendInput(
    const core::protocol::PlayerInputPacket & /*packet*/) {
  // TODO: serialize PlayerInputPacket + enet_peer_send (unreliable channel)
}

void ClientNetwork::poll() {
  // TODO: enet_host_service loop → deserialize → m_listener->on*()
}

} // namespace pacman::client::network
