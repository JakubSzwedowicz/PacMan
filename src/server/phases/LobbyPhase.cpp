#include "server/phases/LobbyPhase.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <algorithm>

#include "core/maps/MapsManager.hpp"

namespace pacman::server::phases {

LobbyPhase::LobbyPhase(network::ServerNetwork &network, std::string mapPath, int maxPlayers)
    : m_network(network), m_mapPath(std::move(mapPath)), m_maxPlayers(maxPlayers) {}

void LobbyPhase::onEnter() {
    LOG_I("LobbyPhase entered");

    auto result = core::maps::MapsManager::loadFromFile(m_mapPath);
    if (!result) {
        LOG_E("Failed to load map '{}': {}", m_mapPath, result.error());
        return;
    }
    m_map = std::move(*result);

    if (m_maxPlayers == 0) {
        LOG_I("maxPlayers=0 — starting game immediately (solo/AI-only mode)");
        requestStartGame();
    }
}

void LobbyPhase::onExit() { LOG_I("LobbyPhase exited"); }

PhaseRequest LobbyPhase::update(float /*dt*/) {
    if (m_pendingRequest) {
        auto req = std::move(*m_pendingRequest);
        m_pendingRequest.reset();
        return req;
    }
    return PhaseRunning{};
}

void LobbyPhase::onUpdate(const network::events::ServerNetworkEvent &event) {
    std::visit(overloaded{[this](const network::events::PlayerConnectedEvent &e) { handleConnect(e.playerId); },
                          [this](const network::events::PlayerDisconnectedEvent &e) { handleDisconnect(e.playerId); },
                          [this](const network::events::LobbyReadyEvent &e) { handleLobbyReady(e.playerId, e.ready); },
                          [](const auto &) {}},
               event);
}

void LobbyPhase::handleConnect(core::PlayerId id) {
    const int spawnCap = m_map ? static_cast<int>(m_map->pacmanSpawns.size()) : core::maxPlayers;
    const int mapCap = (m_map && m_map->maxPlayers > 0) ? m_map->maxPlayers : spawnCap;
    const int effectiveMax =
        (m_maxPlayers > 0) ? std::min({m_maxPlayers, mapCap, spawnCap}) : std::min(mapCap, spawnCap);

    if (m_playerCount >= effectiveMax) {
        LOG_W("Lobby full ({}/{}), rejecting player {}", m_playerCount, effectiveMax, id);
        // TODO (Phase 4): send rejection packet to this peer
        return;
    }

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

void LobbyPhase::handleDisconnect(core::PlayerId id) {
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

void LobbyPhase::handleLobbyReady(core::PlayerId id, bool ready) {
    for (auto &slot : m_slots) {
        if (slot.id == id) {
            slot.ready = ready;
            broadcastLobbyState();
            break;
        }
    }
    // Convention: id==1 is the host (first peer to connect).
    if (id == 1 && allPlayersReady() && m_playerCount > 0) {
        requestStartGame();
    }
}

void LobbyPhase::broadcastLobbyState() {
    core::protocol::LobbyStatePacket pkt;
    for (const auto &slot : m_slots) {
        if (slot.connected) pkt.players.push_back({slot.id, slot.name, true, slot.ready});
    }
    m_network.sendLobbyState(pkt);
}

bool LobbyPhase::allPlayersReady() const {
    for (int i = 0; i < m_playerCount; ++i) {
        if (m_slots[i].connected && !m_slots[i].ready) return false;
    }
    return true;
}

void LobbyPhase::requestStartGame() {
    LOG_I("Requesting game start (playerCount={})", m_playerCount);

    if (!m_map) {
        LOG_E("Cannot start game: map not loaded");
        return;
    }

    std::vector<core::protocol::PlayerInfo> players;
    players.reserve(m_playerCount);
    for (const auto &slot : m_slots) {
        if (slot.connected) players.push_back({slot.id, slot.name, true, false});
    }

    m_pendingRequest = StartGameRequest{std::move(*m_map), std::move(players)};
    m_map.reset();
}

}  // namespace pacman::server::phases
