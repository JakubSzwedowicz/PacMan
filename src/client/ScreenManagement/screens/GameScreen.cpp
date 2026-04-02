#include "client/ScreenManagement/screens/GameScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include <set>
#include <vector>

#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"

namespace pacman::client::screens {

using namespace network::events;

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GameScreen::GameScreen(network::ClientNetwork& network) : m_network(network) { m_ghostEntities.fill(entt::null); }

GameScreen::GameScreen(network::ClientNetwork& network, entt::registry&& registry, core::maps::Map map,
                       std::unordered_map<core::PlayerId, entt::entity> playerEntities,
                       std::array<entt::entity, core::ghostCount> ghostEntities, core::PlayerId localPlayerId,
                       bool isHost)
    : m_network(network),
      m_registry(std::move(registry)),
      m_map(std::move(map)),
      m_networked(true),
      m_localPlayerId(localPlayerId),
      m_isHost(isHost),
      m_playerEntities(std::move(playerEntities)),
      m_ghostEntities(ghostEntities) {
    auto it = m_playerEntities.find(localPlayerId);
    if (it != m_playerEntities.end()) m_localPlayer = it->second;
}

// ---------------------------------------------------------------------------
// Screen lifecycle
// ---------------------------------------------------------------------------

void GameScreen::onEnter() {
    if (m_networked) {
        LOG_I("GameScreen entered (networked, playerId={})", m_localPlayerId);
        return;
    }

    LOG_I("GameScreen entered (offline), loading map: {}", config().mapPath.get());
    auto result = core::maps::MapsManager::loadFromFile(config().mapPath.get());
    if (!result) {
        LOG_E("Failed to load map: {}", result.error());
        queueRequest(screen::OpenMenuRequest{});
        return;
    }
    m_map = std::move(*result);
    spawnEntitiesFromMap();
    LOG_I("GameScreen ready with {} entities", m_registry.storage<entt::entity>().size());
}

void GameScreen::onExit() {
    LOG_I("GameScreen exited");
    m_registry.clear();
    m_localPlayer = entt::null;
}

screen::ScreenRequest GameScreen::update(float dt, const input::InputSnapshot& input) {
    if (input.escapePressed) {
        LOG_I("Escape pressed — returning to menu");
        if (m_networked) m_network.disconnect();
        queueRequest(screen::OpenMenuRequest{});
    }

    if (m_networked) {
        // Send local input to server; simulation runs on the server side.
        if (m_localPlayer != entt::null) {
            core::protocol::PlayerInputPacket pkt{input.tick, m_localPlayerId, input.direction};
            m_network.sendInput(pkt);
        }
        return takeQueuedRequest();
    }

    // Offline / standalone: run simulation locally.
    core::ecs::Input localInput{input.tick, input.direction};
    m_simulation.applyInput(m_registry, m_localPlayer, localInput);
    m_simulation.update(m_registry, dt, m_map);
    return takeQueuedRequest();
}

void GameScreen::draw(sf::RenderWindow& window) {
    m_renderer.render(window, m_registry, m_map);

    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("HUD", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

    if (m_localPlayer != entt::null) {
        auto* ps = m_registry.try_get<core::ecs::PlayerState>(m_localPlayer);
        if (ps) {
            ImGui::Text("Score: %d", ps->score);
            ImGui::Text("Lives: %d", ps->lives);
        }
    }

    ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
    ImGui::End();
}

// ---------------------------------------------------------------------------
// ISubscriber<ClientNetworkEvent>
// ---------------------------------------------------------------------------

void GameScreen::onUpdate(const ClientNetworkEvent& event) {
    std::visit(pacman::overloaded{[this](const GameSnapshotEvent& e) { applySnapshot(e.packet); },
                                  [this](const RoundEndEvent& e) {
                                      LOG_I("Round ended — transitioning to ResultsScreen");
                                      queueRequest(screen::OpenResultsRequest{e.packet, m_localPlayerId, m_isHost});
                                  },
                                  [this](const DisconnectedEvent&) {
                                      LOG_I("Disconnected during game");
                                      queueRequest(screen::OpenMenuRequest{});
                                  },
                                  [this](const ServerShutdownEvent& e) {
                                      LOG_I("Server shutdown during game: {}", e.packet.reason);
                                      queueRequest(screen::OpenMenuRequest{});
                                  },
                                  [](const auto&) {}},
               event);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void GameScreen::applySnapshot(const core::protocol::GameSnapshotPacket& snap) {
    for (const auto& state : snap.players) {
        auto it = m_playerEntities.find(state.id);
        if (it == m_playerEntities.end()) continue;
        entt::entity e = it->second;

        auto [pos, ps, ds] =
            m_registry.try_get<core::ecs::Position, core::ecs::PlayerState, core::ecs::DirectionState>(e);
        if (!pos || !ps || !ds) continue;
        pos->x = state.x;
        pos->y = state.y;
        ps->score = state.score;
        ps->lives = state.lives;
        ds->current = state.dir;
    }

    for (int i = 0; i < core::ghostCount; ++i) {
        entt::entity e = m_ghostEntities[i];
        if (e == entt::null) continue;
        const auto& gs = snap.ghosts[i];

        auto [pos, ghostState, ds] =
            m_registry.try_get<core::ecs::Position, core::ecs::GhostState, core::ecs::DirectionState>(e);
        if (!pos || !ghostState || !ds) continue;
        pos->x = gs.x;
        pos->y = gs.y;
        ghostState->mode = static_cast<core::ecs::GhostState::Mode>(gs.mode);
        ds->current = gs.dir;
    }

    // Reconcile pellets: destroy client-side entities absent from the server's remaining list.
    const float ts = m_map.tileSize;
    auto reconcilePellets = [&]<typename Tag>(const std::vector<core::maps::Tile>& remaining) {
        using Key = std::pair<core::maps::Tile::Unit, core::maps::Tile::Unit>;
        std::set<Key> surviving;
        for (const auto& tile : remaining) surviving.emplace(tile.col(), tile.row());

        std::vector<entt::entity> toDestroy;
        for (auto e : m_registry.view<core::ecs::Position, Tag>()) {
            const auto& pos = m_registry.get<core::ecs::Position>(e);
            Key key{static_cast<core::maps::Tile::Unit>(pos.x / ts),
                    static_cast<core::maps::Tile::Unit>(pos.y / ts)};
            if (!surviving.count(key)) toDestroy.push_back(e);
        }
        if (!toDestroy.empty()) {
            LOG_D("Reconcile: destroying {} pellet entities (server has {})", toDestroy.size(), remaining.size());
        }
        m_registry.destroy(toDestroy.cbegin(), toDestroy.cend());
    };

    reconcilePellets.operator()<core::ecs::PelletTag>(snap.remainingPellets);
    reconcilePellets.operator()<core::ecs::PowerPelletTag>(snap.remainingPowerPellets);
}

void GameScreen::spawnEntitiesFromMap() {
    float ts = m_map.tileSize;

    for (core::maps::Tile::Unit row = 0; row < m_map.height; ++row) {
        for (core::maps::Tile::Unit col = 0; col < m_map.width; ++col) {
            const auto tileType = m_map.tileTypeAt(col, row);
            const float x = static_cast<float>(col) * ts;
            const float y = static_cast<float>(row) * ts;
            if (tileType == core::maps::TileType::Wall) {
                auto wall = m_registry.create();
                m_registry.emplace<core::ecs::Position>(wall, x, y);
                m_registry.emplace<core::ecs::Collider>(wall, ts, ts);
                m_registry.emplace<core::ecs::WallTag>(wall);
            } else if (tileType == core::maps::TileType::Pellet) {
                auto pellet = m_registry.create();
                m_registry.emplace<core::ecs::Position>(pellet, x, y);
                m_registry.emplace<core::ecs::PelletTag>(pellet);
            } else if (tileType == core::maps::TileType::PowerPellet) {
                auto powerPellet = m_registry.create();
                m_registry.emplace<core::ecs::Position>(powerPellet, x, y);
                m_registry.emplace<core::ecs::PowerPelletTag>(powerPellet);
            }
        }
    }

    if (!m_map.pacmanSpawns.empty()) {
        const auto& spawn = m_map.pacmanSpawns[0];
        float x = static_cast<float>(spawn.col()) * ts;
        float y = static_cast<float>(spawn.row()) * ts;

        m_localPlayer = m_registry.create();
        m_registry.emplace<core::ecs::Position>(m_localPlayer, x, y);
        m_registry.emplace<core::ecs::Velocity>(m_localPlayer, core::defaultSpeed);
        m_registry.emplace<core::ecs::DirectionState>(m_localPlayer);
        m_registry.emplace<core::ecs::Collider>(m_localPlayer, ts * 0.9f, ts * 0.9f);
        auto& playerState = m_registry.emplace<core::ecs::PlayerState>(m_localPlayer);
        playerState.spawnTile = spawn;
        m_registry.emplace<core::ecs::PacManTag>(m_localPlayer);
    }
}

}  // namespace pacman::client::screens
