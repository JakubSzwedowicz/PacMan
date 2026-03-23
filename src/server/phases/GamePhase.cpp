#include "server/phases/GamePhase.hpp"
#include "server/app/ServerApp.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::phases {

GamePhase::GamePhase(
    app::ServerApp &app, network::ServerNetwork &network, core::maps::Map map,
    std::array<core::protocol::PlayerInfo, core::maxPlayers> players,
    uint8_t playerCount,
    std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_app(app), m_network(network), m_map(std::move(map)), m_simulation(),
      m_aiSystem(loggerConfig), m_players(players), m_playerCount(playerCount),
      m_logger("GamePhase", std::move(loggerConfig)) {}

void GamePhase::onEnter() {
  LOG_I("GamePhase entered ({} players)", m_playerCount);
  m_network.setHandler(*this);
  spawnEntities();
  // TODO: broadcastGameStart
}

void GamePhase::onExit() {
  LOG_I("GamePhase exited");
  m_network.clearHandler();
  m_registry.clear();
  m_playerEntities.clear();
  m_pendingInputs.clear();
}

void GamePhase::update(float dt) {
  applyPendingInputs();
  m_simulation.update(m_registry, dt, m_map);
  m_aiSystem.update(m_registry, m_map, dt);

  m_snapshotAccumulator += dt;
  if (m_snapshotAccumulator >= snapshotRate) {
    broadcastSnapshot();
    m_snapshotAccumulator -= snapshotRate;
  }

  m_tick++;

  if (isRoundOver()) {
    endRound();
  }
}

void GamePhase::onPlayerDisconnect(core::PlayerId id) {
  LOG_I("Player {} disconnected mid-game", id);
  auto it = m_playerEntities.find(id);
  if (it != m_playerEntities.end()) {
    m_registry.destroy(it->second);
    m_playerEntities.erase(it);
  }
}

void GamePhase::onPlayerInput(const core::protocol::PlayerInputPacket &packet) {
  m_pendingInputs[packet.playerId] = packet;
}

void GamePhase::spawnEntities() {
  // TODO: create wall, pellet, power-pellet entities; spawn one PacMan per
  //       player; spawn four ghost entities
}

void GamePhase::applyPendingInputs() {
  for (auto &[id, pkt] : m_pendingInputs) {
    auto it = m_playerEntities.find(id);
    if (it != m_playerEntities.end()) {
      m_simulation.applyInput(m_registry, it->second, {pkt.tick, pkt.dir});
    }
  }
  m_pendingInputs.clear();
}

void GamePhase::broadcastSnapshot() {
  m_network.broadcastSnapshot(buildSnapshot());
}

bool GamePhase::isRoundOver() const { return false; }

void GamePhase::endRound() {
  LOG_I("Round over");
  m_network.broadcastRoundEnd(buildRoundEnd());
  // TODO: m_app.setPhase(make LobbyPhase)
}

core::protocol::GameSnapshotPacket GamePhase::buildSnapshot() const {
  core::protocol::GameSnapshotPacket snap;
  snap.tick = m_tick;
  snap.playerCount = m_playerCount;
  return snap;
}

core::protocol::RoundEndPacket GamePhase::buildRoundEnd() const {
  core::protocol::RoundEndPacket pkt;
  pkt.playerCount = m_playerCount;
  return pkt;
}

} // namespace pacman::server::phases
