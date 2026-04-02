#include "server/phases/GamePhase.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <algorithm>

#include "core/Common.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::server::phases {

GamePhase::GamePhase(network::ServerNetwork &network, core::maps::Map map,
                     std::vector<core::protocol::PlayerInfo> players, bool renderAscii, int renderIntervalMs)
    : m_network(network),
      m_map(std::move(map)),
      m_players(std::move(players)),
      m_renderAscii(renderAscii),
      m_renderInterval(static_cast<float>(renderIntervalMs) / 1000.0f) {}

void GamePhase::onEnter() {
    LOG_I("GamePhase entered ({} players)", m_players.size());
    spawnEntities();

    if (m_players.empty()) {
        m_allReady = true;  // solo / AI-only mode — start immediately
        return;
    }

    auto mapJson = core::maps::MapsManager::toJson(m_map);
    if (!mapJson) {
        LOG_E("Failed to serialize map for GameStart: {} — aborting to lobby", mapJson.error());
        m_pendingRequest = ReturnToLobbyRequest{};
        return;
    }

    core::protocol::GameStartPacket pkt;
    pkt.mapJson = std::move(*mapJson);
    for (size_t i = 0; i < m_players.size(); ++i) {
        pkt.playerIds.push_back(m_players[i].id);
        pkt.spawnPositions.push_back(m_map.pacmanSpawns[i % m_map.pacmanSpawns.size()]);
    }
    m_network.broadcastGameStart(std::move(pkt));
    LOG_I("GameStart broadcast sent, waiting for {} ReadyToPlay(s)", m_players.size());
}

void GamePhase::onExit() {
    LOG_I("GamePhase exited");
    m_registry.clear();
    m_playerEntities.clear();
    m_pendingInputs.clear();
    m_lastProcessedInputTicks.clear();
}

void GamePhase::onUpdate(const network::events::ServerNetworkEvent &event) {
    std::visit(overloaded{[this](const network::events::PlayerDisconnectedEvent &e) { handleDisconnect(e.playerId); },
                          [this](const network::events::PlayerInputEvent &e) { handleInput(e.packet); },
                          [this](const network::events::ReadyToPlayEvent &) {
                              ++m_readyCount;
                              LOG_I("ReadyToPlay {}/{}", m_readyCount, m_players.size());
                              if (m_readyCount >= m_players.size()) {
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

    const float ts = m_map.tileSize;
    for (auto e : m_registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>()) {
        const auto &pos = m_registry.get<const core::ecs::Position>(e);
        auto &ps = m_registry.get<core::ecs::PlayerState>(e);
        const auto col = static_cast<int32_t>((pos.x + ts * 0.5f) / ts);
        const auto row = static_cast<int32_t>((pos.y + ts * 0.5f) / ts);
        if (col != ps.lastTileCol || row != ps.lastTileRow) {
            ps.lastTileCol = col;
            ps.lastTileRow = row;
            LOG_D("PacMan tile ({},{}) pos=({:.1f},{:.1f})", col, row, pos.x, pos.y);
        }
    }

    m_aiSystem.update(m_registry, m_map, dt);

    auto events = m_rules.applyRules(m_registry, m_map, dt);
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
    m_pendingInputs.erase(id);
    m_lastProcessedInputTicks.erase(id);
    std::erase_if(m_players, [id](const core::protocol::PlayerInfo& player) { return player.id == id; });

    if (!m_allReady && m_readyCount >= m_players.size()) {
        m_allReady = true;
        LOG_I("Remaining players ready after disconnect — simulation starting");
    }

    if (m_playerEntities.empty()) {
        LOG_I("No active players remain — returning to lobby");
        m_pendingRequest = ReturnToLobbyRequest{};
    }
}

void GamePhase::handleInput(const core::protocol::PlayerInputPacket &packet) {
    m_pendingInputs[packet.playerId] = packet;
}

void GamePhase::spawnEntities() {
    float ts = m_map.tileSize;
    const float playerSpeed = core::speedForTileSize(ts);
    const float ghostSpeed = playerSpeed * 0.75f;

    for (size_t r = 0; r < m_map.height; ++r) {
        for (size_t c = 0; c < m_map.width; ++c) {
            const auto tileType = m_map.tileTypeAt(c, r);
            if (tileType == core::maps::TileType::Pellet) {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, static_cast<float>(c) * ts, static_cast<float>(r) * ts);
                m_registry.emplace<core::ecs::PelletTag>(e);
            } else if (tileType == core::maps::TileType::PowerPellet) {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, static_cast<float>(c) * ts, static_cast<float>(r) * ts);
                m_registry.emplace<core::ecs::PowerPelletTag>(e);
            }
        }
    }

    for (size_t i = 0; i < m_players.size(); ++i) {
        core::PlayerId pid = m_players[i].id;
        const auto &spawn = m_map.pacmanSpawns[i % m_map.pacmanSpawns.size()];

        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, static_cast<float>(spawn.col()) * ts,
                                                static_cast<float>(spawn.row()) * ts);
        m_registry.emplace<core::ecs::Velocity>(e, playerSpeed);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts, ts);
        auto& playerState = m_registry.emplace<core::ecs::PlayerState>(e);
        playerState.spawnTile = spawn;
        m_registry.emplace<core::ecs::PacManTag>(e);
        m_playerEntities[pid] = e;
        m_lastProcessedInputTicks[pid] = 0;
    }

    const bool hasGhostHouse = !(m_map.ghostHouseExit.col() == 0 && m_map.ghostHouseExit.row() == 0);
    const auto initialGhostMode =
        hasGhostHouse ? core::ecs::GhostState::Mode::InHouse : core::ecs::GhostState::Mode::Scatter;

    auto spawnGhost = [&](core::ecs::GhostType type, const core::maps::Tile &tile) {
        if (tile.col() == 0 && tile.row() == 0) return;
        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, static_cast<float>(tile.col()) * ts,
                                                static_cast<float>(tile.row()) * ts);
        m_registry.emplace<core::ecs::Velocity>(e, ghostSpeed);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts, ts);
        auto &gs = m_registry.emplace<core::ecs::GhostState>(e, initialGhostMode, type);
        gs.spawnTile = tile;  // Remember spawn position for eaten ghost return
        m_registry.emplace<core::ecs::GhostTag>(e);
    };

    const auto &gs = m_map.ghostSpawns;
    spawnGhost(core::ecs::GhostType::Blinky, gs.blinky);
    spawnGhost(core::ecs::GhostType::Pinky, gs.pinky);
    spawnGhost(core::ecs::GhostType::Inky, gs.inky);
    spawnGhost(core::ecs::GhostType::Clyde, gs.clyde);

    LOG_I("Spawned entities: {} PacMen, ghosts from map", m_players.size());
}

void GamePhase::applyPendingInputs() {
    for (auto &[id, pkt] : m_pendingInputs) {
        auto it = m_playerEntities.find(id);
        if (it != m_playerEntities.end()) {
            m_simulation.applyInput(m_registry, it->second, {pkt.tick, pkt.dir});
            m_lastProcessedInputTicks[id] = std::max(m_lastProcessedInputTicks[id], pkt.tick);
        }
    }
    m_pendingInputs.clear();
}

void GamePhase::broadcastSnapshot() { m_network.broadcastSnapshot(buildSnapshot()); }

bool GamePhase::isRoundOver() const {
    if (m_players.empty()) return false;

    if (m_registry.view<const core::ecs::PelletTag>().empty() &&
        m_registry.view<const core::ecs::PowerPelletTag>().empty())
        return true;

    for (auto e : m_registry.view<const core::ecs::PlayerState, const core::ecs::PacManTag>()) {
        if (m_registry.get<const core::ecs::PlayerState>(e).lives > 0) return false;
    }
    return true;
}

void GamePhase::endRound() {
    const bool noPellets = m_registry.view<const core::ecs::PelletTag>().empty() &&
                           m_registry.view<const core::ecs::PowerPelletTag>().empty();
    LOG_I("Round over (tick={}) — reason: {}", m_tick, noPellets ? "all pellets eaten" : "all players out of lives");
    m_network.broadcastRoundEnd(buildRoundEnd());
    m_pendingRequest = ReturnToLobbyRequest{};
}

core::protocol::GameSnapshotPacket GamePhase::buildSnapshot() const {
    core::protocol::GameSnapshotPacket snap;
    snap.tick = m_tick;

    std::unordered_map<core::PlayerId, std::string> playerNames;
    playerNames.reserve(m_players.size());
    for (const auto &player : m_players) {
        playerNames.emplace(player.id, player.name);
    }

    for (const auto &[pid, entity] : m_playerEntities) {
        if (!m_registry.valid(entity)) continue;
        const auto &pos = m_registry.get<core::ecs::Position>(entity);
        const auto &dir = m_registry.get<core::ecs::DirectionState>(entity);
        const auto &state = m_registry.get<core::ecs::PlayerState>(entity);
        const auto nameIt = playerNames.find(pid);
        const auto processedIt = m_lastProcessedInputTicks.find(pid);
        snap.players.push_back({pid,
                                nameIt != playerNames.end() ? nameIt->second : "Player" + std::to_string(pid),
                                pos.x,
                                pos.y,
                                dir.current,
                                processedIt != m_lastProcessedInputTicks.end() ? processedIt->second : 0,
                                state.score,
                                state.lives,
                                state.lives > 0});
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

    const float ts = m_map.tileSize;
    for (auto e : m_registry.view<const core::ecs::Position, const core::ecs::PelletTag>()) {
        const auto &pos = m_registry.get<core::ecs::Position>(e);
        snap.remainingPellets.push_back(core::maps::Tile{
            {static_cast<core::maps::Tile::Unit>(pos.x / ts), static_cast<core::maps::Tile::Unit>(pos.y / ts)}});
    }
    for (auto e : m_registry.view<const core::ecs::Position, const core::ecs::PowerPelletTag>()) {
        const auto &pos = m_registry.get<core::ecs::Position>(e);
        snap.remainingPowerPellets.push_back(core::maps::Tile{
            {static_cast<core::maps::Tile::Unit>(pos.x / ts), static_cast<core::maps::Tile::Unit>(pos.y / ts)}});
    }

    return snap;
}

core::protocol::RoundEndPacket GamePhase::buildRoundEnd() const {
    core::protocol::RoundEndPacket pkt;

    std::unordered_map<core::PlayerId, std::string> playerNames;
    playerNames.reserve(m_players.size());
    for (const auto &player : m_players) {
        playerNames.emplace(player.id, player.name);
    }

    int maxScore = -1;
    for (const auto &[pid, entity] : m_playerEntities) {
        if (!m_registry.valid(entity)) continue;
        const auto &pos = m_registry.get<core::ecs::Position>(entity);
        const auto &dir = m_registry.get<core::ecs::DirectionState>(entity);
        const auto &state = m_registry.get<core::ecs::PlayerState>(entity);
        const auto nameIt = playerNames.find(pid);
        pkt.finalScores.push_back({pid,
                                   nameIt != playerNames.end() ? nameIt->second : "Player" + std::to_string(pid),
                                   pos.x,
                                   pos.y,
                                   dir.current,
                                   0,
                                   state.score,
                                   state.lives,
                                   state.lives > 0});
        if (state.score > maxScore) {
            maxScore = state.score;
            pkt.winnerId = pid;
        }
    }
    return pkt;
}

}  // namespace pacman::server::phases
