#include "server/network/ServerNetwork.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace {
// Null Object — absorbs all events silently when no phase is active.
class NullNetworkEventHandler
    : public pacman::server::network::INetworkEventHandler {};

NullNetworkEventHandler g_nullHandler;
} // namespace

namespace pacman::server::network {

struct ServerNetwork::Impl {
  // TODO: ENet host + peer map (PlayerId → ENetPeer*)
};

ServerNetwork::ServerNetwork()
    : m_impl(std::make_unique<Impl>()) {
  clearHandler();
  LOG_I("ServerNetwork created");
}

ServerNetwork::~ServerNetwork() { stop(); }

bool ServerNetwork::start(uint16_t port, int maxClients) {
  LOG_I("Starting on port {} (maxClients={})", port, maxClients);
  // TODO (Phase 4): enet_host_create — ENet not yet wired up.
  // Return true so the game loop proceeds in server-solo mode (Phase 3).
  (void)port;
  (void)maxClients;
  return true;
}

void ServerNetwork::stop() {
  // TODO: enet_host_destroy
}

bool ServerNetwork::isRunning() const { return false; }

void ServerNetwork::setHandler(INetworkEventHandler &handler) {
  m_handler = &handler;
}

void ServerNetwork::clearHandler() { m_handler = &g_nullHandler; }

void ServerNetwork::sendLobbyState(
    const core::protocol::LobbyStatePacket & /*p*/) {}

void ServerNetwork::broadcastGameStart(
    const core::protocol::GameStartPacket & /*templatePkt*/,
    const std::array<core::PlayerId, core::maxPlayers> & /*playerIds*/,
    uint8_t /*count*/) {
  // TODO: for each connected peer, copy templatePkt, set assignedPlayerId, send
}

void ServerNetwork::broadcastSnapshot(
    const core::protocol::GameSnapshotPacket & /*p*/) {}
void ServerNetwork::broadcastRoundEnd(
    const core::protocol::RoundEndPacket & /*p*/) {}
void ServerNetwork::broadcastShutdown(
    const core::protocol::ServerShutdownPacket & /*p*/) {}

void ServerNetwork::poll() {
  // TODO: enet_host_service loop → deserialize → m_handler->on*()
}

} // namespace pacman::server::network
