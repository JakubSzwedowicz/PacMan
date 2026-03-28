#include "server/phases/GamePhase.hpp"
#include "server/app/ServerApp.hpp"
#include "server/phases/LobbyPhase.hpp"

#include "core/Common.hpp"
#include "core/ecs/Components.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::server::phases {

GamePhase::GamePhase(app::ServerApp &app, network::ServerNetwork &network,
                     core::maps::Map map,
                     std::array<core::protocol::PlayerInfo, core::maxPlayers> players,
                     uint8_t playerCount)
    : m_app(app), m_network(network), m_map(std::move(map)),
      m_players(players), m_playerCount(playerCount),
      m_renderAscii(app.config().renderAscii.get()),
      m_renderInterval(static_cast<float>(app.config().renderInterval.get()) / 1000.0f) {}

void GamePhase::onEnter() {
  LOG_I("GamePhase entered ({} players)", m_playerCount);
  m_network.setHandler(*this);
  spawnEntities();
  // TODO (Phase 4): broadcastGameStart
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

  auto events = m_rules.applyRules(m_registry, m_map);
  if (events.powerPelletEaten) {
    m_aiSystem.onPowerPelletEaten();
  }

  m_snapshotAccumulator += dt;
  if (m_snapshotAccumulator >= snapshotRate) {
    broadcastSnapshot();
    m_snapshotAccumulator -= snapshotRate;
  }

  if (m_renderAscii) {
    m_renderAccumulator += dt;
    if (m_renderAccumulator >= m_renderInterval) {
      m_asciiRenderer.render(m_registry, m_map);
      m_renderAccumulator -= m_renderInterval;
    }
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
  float ts = m_map.tileSize;

  // Pellets and power pellets from map tiles.
  for (size_t r = 0; r < m_map.height; ++r) {
    for (size_t c = 0; c < m_map.width; ++c) {
      char tile = m_map.tileAt(c, r);
      if (tile == '.') {
        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, static_cast<float>(c) * ts,
                                                 static_cast<float>(r) * ts);
        m_registry.emplace<core::ecs::PelletTag>(e);
      } else if (tile == 'o') {
        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, static_cast<float>(c) * ts,
                                                 static_cast<float>(r) * ts);
        m_registry.emplace<core::ecs::PowerPelletTag>(e);
      }
    }
  }

  // One PacMan per player.
  for (uint8_t i = 0; i < m_playerCount; ++i) {
    core::PlayerId pid = m_players[i].id;
    const auto &spawn =
        m_map.pacmanSpawns[i % m_map.pacmanSpawns.size()];

    auto e = m_registry.create();
    m_registry.emplace<core::ecs::Position>(
        e, static_cast<float>(spawn.col()) * ts,
        static_cast<float>(spawn.row()) * ts);
    m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed);
    m_registry.emplace<core::ecs::DirectionState>(e);
    m_registry.emplace<core::ecs::Collider>(e, ts, ts);
    m_registry.emplace<core::ecs::PlayerState>(e);
    m_registry.emplace<core::ecs::PacManTag>(e);
    m_playerEntities[pid] = e;
  }

  // Ghost entities from map spawns.
  auto spawnGhost = [&](core::ecs::GhostType type,
                         const core::maps::Tile &tile) {
    if (tile.col() == 0 && tile.row() == 0) return; // sentinel = not used
    auto e = m_registry.create();
    m_registry.emplace<core::ecs::Position>(
        e, static_cast<float>(tile.col()) * ts,
        static_cast<float>(tile.row()) * ts);
    m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed * 0.75f);
    m_registry.emplace<core::ecs::DirectionState>(e);
    m_registry.emplace<core::ecs::Collider>(e, ts, ts);
    m_registry.emplace<core::ecs::GhostState>(
        e, core::ecs::GhostState::Mode::Scatter, type);
    m_registry.emplace<core::ecs::GhostTag>(e);
  };

  const auto &gs = m_map.ghostSpawns;
  spawnGhost(core::ecs::GhostType::Blinky, gs.blinky);
  spawnGhost(core::ecs::GhostType::Pinky,  gs.pinky);
  spawnGhost(core::ecs::GhostType::Inky,   gs.inky);
  spawnGhost(core::ecs::GhostType::Clyde,  gs.clyde);

  LOG_I("Spawned entities: {} PacMen, ghosts from map", m_playerCount);
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

bool GamePhase::isRoundOver() const {
  // No players in server-solo mode — run until stopped.
  if (m_playerCount == 0) return false;

  // All pellets eaten → players win.
  bool noPellets = true;
  for (auto e : m_registry.view<const core::ecs::PelletTag>()) {
    (void)e;
    noPellets = false;
    break;
  }
  if (noPellets) {
    for (auto e : m_registry.view<const core::ecs::PowerPelletTag>()) {
      (void)e;
      noPellets = false;
      break;
    }
  }
  if (noPellets) return true;

  // All players out of lives → game over.
  bool anyAlive = false;
  for (auto e :
       m_registry.view<const core::ecs::PlayerState, const core::ecs::PacManTag>()) {
    if (m_registry.get<const core::ecs::PlayerState>(e).lives > 0) {
      anyAlive = true;
      break;
    }
  }
  return !anyAlive;
}

void GamePhase::endRound() {
  LOG_I("Round over (tick={})", m_tick);
  m_network.broadcastRoundEnd(buildRoundEnd());
  m_app.setPhase(std::make_unique<LobbyPhase>(m_app, m_network));
}

core::protocol::GameSnapshotPacket GamePhase::buildSnapshot() const {
  core::protocol::GameSnapshotPacket snap;
  snap.tick = m_tick;
  snap.playerCount = m_playerCount;

  uint8_t pi = 0;
  for (const auto &[pid, entity] : m_playerEntities) {
    if (!m_registry.valid(entity) || pi >= core::maxPlayers) continue;
    const auto &pos = m_registry.get<core::ecs::Position>(entity);
    const auto &dir = m_registry.get<core::ecs::DirectionState>(entity);
    const auto &state = m_registry.get<core::ecs::PlayerState>(entity);
    snap.players[pi++] = {pid,       pos.x,        pos.y,
                          dir.current, state.score, state.lives,
                          state.lives > 0};
  }

  uint8_t gi = 0;
  for (auto e :
       m_registry.view<const core::ecs::Position, const core::ecs::DirectionState,
                        const core::ecs::GhostState, const core::ecs::GhostTag>()) {
    if (gi >= core::ghostCount) break;
    const auto &pos = m_registry.get<core::ecs::Position>(e);
    const auto &dir = m_registry.get<core::ecs::DirectionState>(e);
    const auto &gs = m_registry.get<core::ecs::GhostState>(e);
    snap.ghosts[gi++] = {gs.type, pos.x, pos.y, dir.current,
                          static_cast<uint8_t>(gs.mode)};
  }

  return snap;
}

core::protocol::RoundEndPacket GamePhase::buildRoundEnd() const {
  core::protocol::RoundEndPacket pkt;
  pkt.playerCount = m_playerCount;

  uint8_t i = 0;
  int maxScore = -1;
  for (const auto &[pid, entity] : m_playerEntities) {
    if (!m_registry.valid(entity) || i >= core::maxPlayers) continue;
    const auto &pos = m_registry.get<core::ecs::Position>(entity);
    const auto &dir = m_registry.get<core::ecs::DirectionState>(entity);
    const auto &state = m_registry.get<core::ecs::PlayerState>(entity);
    pkt.finalScores[i++] = {pid,         pos.x,       pos.y,
                             dir.current, state.score, state.lives,
                             state.lives > 0};
    if (state.score > maxScore) {
      maxScore = state.score;
      pkt.winnerId = pid;
    }
  }
  return pkt;
}

} // namespace pacman::server::phases
