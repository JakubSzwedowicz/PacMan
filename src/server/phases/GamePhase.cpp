#include "server/phases/GamePhase.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/Common.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::server::phases {

GamePhase::GamePhase(network::ServerNetwork &network, core::maps::Map map,
                     std::array<core::protocol::PlayerInfo, core::maxPlayers> players, uint8_t playerCount,
                     bool renderAscii, int renderIntervalMs)
    : m_network(network),
      m_map(std::move(map)),
      m_players(players),
      m_playerCount(playerCount),
      m_renderAscii(renderAscii),
      m_renderInterval(static_cast<float>(renderIntervalMs) / 1000.0f) {}

void GamePhase::onEnter() {
    LOG_I("GamePhase entered ({} players)", m_playerCount);
    spawnEntities();

    if (m_playerCount == 0) {
        m_allReady = true;  // solo / AI-only mode — start immediately
        return;
    }

    auto mapJson = core::maps::MapsManager::toJson(m_map);
    if (!mapJson) {
        LOG_E("Failed to serialize map for GameStart: {}", mapJson.error());
        m_allReady = true;  // degraded: start without waiting
        return;
    }

    core::protocol::GameStartPacket pkt;
    pkt.mapJson = std::move(*mapJson);
    pkt.playerCount = m_playerCount;
    std::array<core::PlayerId, core::maxPlayers> playerIds{};
    for (int i = 0; i < m_playerCount; ++i) {
        pkt.playerIds[i] = m_players[i].id;
        pkt.spawnPositions[i] = m_map.pacmanSpawns[i % m_map.pacmanSpawns.size()];
        playerIds[i] = m_players[i].id;
    }
    m_network.broadcastGameStart(pkt, playerIds, m_playerCount);
    LOG_I("GameStart broadcast sent, waiting for {} ReadyToPlay(s)", m_playerCount);
}

void GamePhase::onExit() {
    LOG_I("GamePhase exited");
    m_registry.clear();
    m_playerEntities.clear();
    m_pendingInputs.clear();
}

void GamePhase::onUpdate(const network::events::ServerNetworkEvent &event) {
    std::visit(overloaded{[this](const network::events::PlayerDisconnectedEvent &e) { handleDisconnect(e.playerId); },
                          [this](const network::events::PlayerInputEvent &e) { handleInput(e.packet); },
                          [this](const network::events::ReadyToPlayEvent &) {
                              ++m_readyCount;
                              LOG_I("ReadyToPlay {}/{}", m_readyCount, m_playerCount);
                              if (m_readyCount >= m_playerCount) {
                                  m_allReady = true;
                                  LOG_I("All players ready — simulation starting");
                              }
                          },
                          [](const auto &) {}},
               event);
}

PhaseRequest GamePhase::update(float dt) {
    if (!m_allReady) {
        if (m_pendingRequest) {
            auto req = std::move(*m_pendingRequest);
            m_pendingRequest.reset();
            return req;
        }
        return PhaseRunning{};
    }

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

    if (m_pendingRequest) {
        auto req = std::move(*m_pendingRequest);
        m_pendingRequest.reset();
        return req;
    }
    return PhaseRunning{};
}

void GamePhase::handleDisconnect(core::PlayerId id) {
    LOG_I("Player {} disconnected mid-game", id);
    auto it = m_playerEntities.find(id);
    if (it != m_playerEntities.end()) {
        m_registry.destroy(it->second);
        m_playerEntities.erase(it);
    }
}

void GamePhase::handleInput(const core::protocol::PlayerInputPacket &packet) {
    m_pendingInputs[packet.playerId] = packet;
}

void GamePhase::spawnEntities() {
    float ts = m_map.tileSize;

    for (size_t r = 0; r < m_map.height; ++r) {
        for (size_t c = 0; c < m_map.width; ++c) {
            char tile = m_map.tileAt(c, r);
            if (tile == '.') {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, static_cast<float>(c) * ts, static_cast<float>(r) * ts);
                m_registry.emplace<core::ecs::PelletTag>(e);
            } else if (tile == 'o') {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, static_cast<float>(c) * ts, static_cast<float>(r) * ts);
                m_registry.emplace<core::ecs::PowerPelletTag>(e);
            }
        }
    }

    for (uint8_t i = 0; i < m_playerCount; ++i) {
        core::PlayerId pid = m_players[i].id;
        const auto &spawn = m_map.pacmanSpawns[i % m_map.pacmanSpawns.size()];

        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, static_cast<float>(spawn.col()) * ts,
                                                static_cast<float>(spawn.row()) * ts);
        m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts, ts);
        m_registry.emplace<core::ecs::PlayerState>(e);
        m_registry.emplace<core::ecs::PacManTag>(e);
        m_playerEntities[pid] = e;
    }

    auto spawnGhost = [&](core::ecs::GhostType type, const core::maps::Tile &tile) {
        if (tile.col() == 0 && tile.row() == 0) return;
        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, static_cast<float>(tile.col()) * ts,
                                                static_cast<float>(tile.row()) * ts);
        m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed * 0.75f);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts, ts);
        m_registry.emplace<core::ecs::GhostState>(e, core::ecs::GhostState::Mode::Scatter, type);
        m_registry.emplace<core::ecs::GhostTag>(e);
    };

    const auto &gs = m_map.ghostSpawns;
    spawnGhost(core::ecs::GhostType::Blinky, gs.blinky);
    spawnGhost(core::ecs::GhostType::Pinky, gs.pinky);
    spawnGhost(core::ecs::GhostType::Inky, gs.inky);
    spawnGhost(core::ecs::GhostType::Clyde, gs.clyde);

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

void GamePhase::broadcastSnapshot() { m_network.broadcastSnapshot(buildSnapshot()); }

bool GamePhase::isRoundOver() const {
    if (m_playerCount == 0) return false;

    bool noPellets = m_registry.view<const core::ecs::PelletTag>().empty() &&
                     m_registry.view<const core::ecs::PowerPelletTag>().empty();
    if (noPellets) return true;

    for (auto e : m_registry.view<const core::ecs::PlayerState, const core::ecs::PacManTag>()) {
        if (m_registry.get<const core::ecs::PlayerState>(e).lives > 0) return false;
    }
    return true;
}

void GamePhase::endRound() {
    LOG_I("Round over (tick={})", m_tick);
    m_network.broadcastRoundEnd(buildRoundEnd());
    m_pendingRequest = ReturnToLobbyRequest{};
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
        snap.players[pi++] = {pid, pos.x, pos.y, dir.current, state.score, state.lives, state.lives > 0};
    }

    uint8_t gi = 0;
    for (auto e : m_registry.view<const core::ecs::Position, const core::ecs::DirectionState,
                                  const core::ecs::GhostState, const core::ecs::GhostTag>()) {
        if (gi >= core::ghostCount) break;
        const auto &pos = m_registry.get<core::ecs::Position>(e);
        const auto &dir = m_registry.get<core::ecs::DirectionState>(e);
        const auto &gs = m_registry.get<core::ecs::GhostState>(e);
        snap.ghosts[gi++] = {gs.type, pos.x, pos.y, dir.current, static_cast<uint8_t>(gs.mode)};
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
        pkt.finalScores[i++] = {pid, pos.x, pos.y, dir.current, state.score, state.lives, state.lives > 0};
        if (state.score > maxScore) {
            maxScore = state.score;
            pkt.winnerId = pid;
        }
    }
    return pkt;
}

}  // namespace pacman::server::phases
