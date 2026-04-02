#include "client/ScreenManagement/screens/GameScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include <algorithm>
#include <cmath>
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
                       std::array<entt::entity, core::ghostCount> ghostEntities,
                       std::optional<core::protocol::GameSnapshotPacket> initialSnapshot, core::PlayerId localPlayerId,
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
    if (initialSnapshot) applySnapshot(*initialSnapshot);
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
    m_inputHistory.clear();
    m_remoteSnapshots.clear();
    m_localPlayer = entt::null;
}

screen::ScreenRequest GameScreen::update(float dt, const input::InputSnapshot& input) {
    m_latestClientTick = input.tick;

    if (input.escapePressed) {
        LOG_I("Escape pressed — returning to menu");
        if (m_networked) m_network.disconnect();
        queueRequest(screen::OpenMenuRequest{});
    }

    if (m_networked) {
        if (m_localPlayer != entt::null) {
            m_inputHistory.push_back({input.tick, input.direction});
            constexpr size_t kMaxInputHistory = 240;
            while (m_inputHistory.size() > kMaxInputHistory) {
                m_inputHistory.pop_front();
            }
            core::protocol::PlayerInputPacket pkt{input.tick, m_localPlayerId, input.direction};
            m_network.sendInput(pkt);
            applyLocalPrediction(input);
        }
        updateRemoteEntities();
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
        ps->score = state.score;
        ps->lives = state.lives;
        ds->current = state.dir;

        if (state.id == m_localPlayerId) {
            reconcileLocalPlayer(state);
        } else {
            pushRemoteSnapshot(e, {snap.tick, state.x, state.y, state.dir});
        }
    }

    for (int i = 0; i < core::ghostCount; ++i) {
        entt::entity e = m_ghostEntities[i];
        if (e == entt::null) continue;
        const auto& gs = snap.ghosts[i];

        auto [pos, ghostState, ds] =
            m_registry.try_get<core::ecs::Position, core::ecs::GhostState, core::ecs::DirectionState>(e);
        if (!pos || !ghostState || !ds) continue;
        ghostState->mode = static_cast<core::ecs::GhostState::Mode>(gs.mode);
        ds->current = gs.dir;
        pushRemoteSnapshot(e, {snap.tick, gs.x, gs.y, gs.dir});
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

void GameScreen::applyLocalPrediction(const input::InputSnapshot& input) {
    if (m_localPlayer == entt::null || !m_registry.valid(m_localPlayer)) return;
    m_simulation.applyInput(m_registry, m_localPlayer, {input.tick, input.direction});
    m_simulation.updateEntity(m_registry, m_localPlayer, core::tickDt, m_map);
}

void GameScreen::reconcileLocalPlayer(const core::protocol::EntityState& authoritativeState) {
    if (m_localPlayer == entt::null || !m_registry.valid(m_localPlayer)) return;

    auto [pos, dirState] = m_registry.try_get<core::ecs::Position, core::ecs::DirectionState>(m_localPlayer);
    if (!pos || !dirState) return;

    while (!m_inputHistory.empty() && m_inputHistory.front().tick <= authoritativeState.lastProcessedTick) {
        m_inputHistory.pop_front();
    }

    pos->x = authoritativeState.x;
    pos->y = authoritativeState.y;
    dirState->current = authoritativeState.dir;
    dirState->next = core::ecs::Direction::None;

    for (const auto& record : m_inputHistory) {
        m_simulation.applyInput(m_registry, m_localPlayer, {record.tick, record.direction});
        m_simulation.updateEntity(m_registry, m_localPlayer, core::tickDt, m_map);
    }
}

void GameScreen::pushRemoteSnapshot(entt::entity entity, const BufferedSnapshot& snapshot) {
    if (!m_registry.valid(entity)) return;
    auto& queue = m_remoteSnapshots[entity];
    if (!queue.empty() && queue.back().tick == snapshot.tick) {
        queue.back() = snapshot;
    } else {
        queue.push_back(snapshot);
    }

    constexpr size_t kMaxBufferedSnapshots = 8;
    while (queue.size() > kMaxBufferedSnapshots) {
        queue.pop_front();
    }
}

void GameScreen::updateRemoteEntities() {
    constexpr core::Tick kInterpolationDelayTicks = 3;
    constexpr core::Tick kMaxExtrapolationTicks = 4;

    const core::Tick renderTick =
        (m_latestClientTick > kInterpolationDelayTicks) ? (m_latestClientTick - kInterpolationDelayTicks) : 0;

    for (auto& [entity, snapshots] : m_remoteSnapshots) {
        if (!m_registry.valid(entity) || snapshots.empty()) continue;

        auto* pos = m_registry.try_get<core::ecs::Position>(entity);
        auto* velocity = m_registry.try_get<core::ecs::Velocity>(entity);
        if (!pos) continue;

        while (snapshots.size() >= 2 && snapshots[1].tick <= renderTick) {
            snapshots.pop_front();
        }

        if (renderTick < snapshots.front().tick) {
            pos->x = snapshots.front().x;
            pos->y = snapshots.front().y;
            continue;
        }

        if (snapshots.size() >= 2 && renderTick >= snapshots.front().tick && renderTick <= snapshots[1].tick) {
            const auto& a = snapshots.front();
            const auto& b = snapshots[1];
            const float denom = static_cast<float>(std::max<core::Tick>(1, b.tick - a.tick));
            const float alpha = std::clamp(static_cast<float>(renderTick - a.tick) / denom, 0.0f, 1.0f);
            pos->x = a.x + (b.x - a.x) * alpha;
            pos->y = a.y + (b.y - a.y) * alpha;
            continue;
        }

        const auto& latest = snapshots.back();
        pos->x = latest.x;
        pos->y = latest.y;

        if (renderTick <= latest.tick || !velocity) continue;

        const core::Tick extrapolatedTicks = std::min(renderTick - latest.tick, kMaxExtrapolationTicks);
        const float distance = velocity->speed * (static_cast<float>(extrapolatedTicks) * core::tickDt);
        switch (latest.dir) {
            case core::ecs::Direction::Up:
                pos->y -= distance;
                break;
            case core::ecs::Direction::Down:
                pos->y += distance;
                break;
            case core::ecs::Direction::Left:
                pos->x -= distance;
                break;
            case core::ecs::Direction::Right:
                pos->x += distance;
                break;
            case core::ecs::Direction::None:
                break;
        }
    }
}

void GameScreen::spawnEntitiesFromMap() {
    float ts = m_map.tileSize;
    const float playerSpeed = core::speedForTileSize(ts);

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
        m_registry.emplace<core::ecs::Velocity>(m_localPlayer, playerSpeed);
        m_registry.emplace<core::ecs::DirectionState>(m_localPlayer);
        m_registry.emplace<core::ecs::Collider>(m_localPlayer, ts * 0.9f, ts * 0.9f);
        auto& playerState = m_registry.emplace<core::ecs::PlayerState>(m_localPlayer);
        playerState.spawnTile = spawn;
        m_registry.emplace<core::ecs::PacManTag>(m_localPlayer);
    }
}

}  // namespace pacman::client::screens
