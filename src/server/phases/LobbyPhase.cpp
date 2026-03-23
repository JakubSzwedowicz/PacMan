#include "server/phases/LobbyPhase.hpp"
#include "server/app/ServerApp.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::phases {

LobbyPhase::LobbyPhase(
    app::ServerApp &app, network::ServerNetwork &network,
    const core::ServerConfig &config,
    std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_app(app), m_network(network), m_config(config),
      m_logger("LobbyPhase", std::move(loggerConfig)) {}

void LobbyPhase::onEnter() {
  LOG_I("LobbyPhase entered");
  m_network.setHandler(*this);
}

void LobbyPhase::onExit() {
  LOG_I("LobbyPhase exited");
  m_network.clearHandler();
}

void LobbyPhase::update(float /*dt*/) {}

void LobbyPhase::onPlayerConnect(core::PlayerId id) {
  LOG_I("Player {} connected", id);
  for (auto &slot : m_slots) {
    if (!slot.connected) {
      slot.id = id;
      slot.connected = true;
      slot.ready = false;
      m_playerCount++;
      broadcastLobbyState();
      return;
    }
  }
  LOG_E("No free slot for player {}", id);
}

void LobbyPhase::onPlayerDisconnect(core::PlayerId id) {
  LOG_I("Player {} disconnected", id);
  for (auto &slot : m_slots) {
    if (slot.id == id) {
      slot = {};
      m_playerCount--;
      broadcastLobbyState();
      return;
    }
  }
}

void LobbyPhase::onLobbyReady(core::PlayerId id, bool ready) {
  for (auto &slot : m_slots) {
    if (slot.id == id) {
      slot.ready = ready;
      broadcastLobbyState();
      break;
    }
  }
  // Convention: id==1 is the host (first peer to connect in the listen-server model).
  // TODO: track host ID explicitly once ServerNetwork assigns peer IDs.
  if (id == 1 && allPlayersReady() && m_playerCount > 0) {
    startGame();
  }
}

void LobbyPhase::broadcastLobbyState() {
  core::protocol::LobbyStatePacket pkt;
  pkt.playerCount = m_playerCount;
  for (int i = 0; i < core::maxPlayers; ++i) {
    pkt.players[i] = {m_slots[i].id, m_slots[i].name, m_slots[i].connected,
                      m_slots[i].ready};
  }
  m_network.sendLobbyState(pkt);
}

bool LobbyPhase::allPlayersReady() const {
  for (int i = 0; i < m_playerCount; ++i) {
    if (m_slots[i].connected && !m_slots[i].ready) {
      return false;
    }
  }
  return true;
}

void LobbyPhase::startGame() {
  LOG_I("All players ready — starting game");
  // TODO: build player array, load map, create GamePhase, m_app.setPhase(...)
}

} // namespace pacman::server::phases
