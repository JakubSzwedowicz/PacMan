#include "server/phases/LobbyPhase.hpp"
#include "server/app/ServerApp.hpp"
#include "server/phases/GamePhase.hpp"

#include "core/maps/MapsManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::phases {

LobbyPhase::LobbyPhase(app::ServerApp &app, network::ServerNetwork &network)
    : m_app(app), m_network(network) {}

void LobbyPhase::onEnter() {
  LOG_I("LobbyPhase entered");
  m_network.setHandler(*this);

  auto result = core::maps::MapsManager::loadFromFile(
      m_app.config().mapPath.get());
  if (!result) {
    LOG_E("Failed to load map '{}': {}", m_app.config().mapPath.get(),
          result.error());
    return;
  }
  m_map = std::move(*result);

  if (m_map->minPlayers == 0) {
    LOG_I("minPlayers=0 — starting game immediately (server solo mode)");
    startGame();
  }
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
    if (m_slots[i].connected && !m_slots[i].ready) return false;
  }
  return true;
}

void LobbyPhase::startGame() {
  LOG_I("Starting game (playerCount={})", m_playerCount);

  if (!m_map) {
    LOG_E("Cannot start game: map not loaded");
    return;
  }

  std::array<core::protocol::PlayerInfo, core::maxPlayers> players{};
  for (int i = 0; i < m_playerCount; ++i) {
    players[i] = {m_slots[i].id, m_slots[i].name, true, false};
  }

  m_app.setPhase(std::make_unique<GamePhase>(m_app, m_network, std::move(*m_map),
                                              players, m_playerCount));
  m_map.reset();
}

} // namespace pacman::server::phases
