#include "client/screens/GameScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include "client/screen/ScreenManager.hpp"
#include "client/screens/MenuScreen.hpp"
#include "client/screens/ResultsScreen.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"

namespace pacman::client::screens {

using namespace network::events;

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GameScreen::GameScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network,
                       std::string mapPath, std::string serverAddress, int serverPort)
    : m_screenManager(screenManager),
      m_network(network),
      m_mapPath(std::move(mapPath)),
      m_serverAddress(std::move(serverAddress)),
      m_serverPort(serverPort) {
    m_ghostEntities.fill(entt::null);
}

GameScreen::GameScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network,
                       entt::registry &&registry, core::maps::Map map,
                       std::unordered_map<core::PlayerId, entt::entity> playerEntities,
                       std::array<entt::entity, core::ghostCount> ghostEntities,
                       core::PlayerId localPlayerId, bool isHost,
                       std::string mapPath, std::string serverAddress, int serverPort)
    : m_screenManager(screenManager),
      m_network(network),
      m_mapPath(std::move(mapPath)),
      m_serverAddress(std::move(serverAddress)),
      m_serverPort(serverPort),
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

    LOG_I("GameScreen entered (offline), loading map: {}", m_mapPath);
    auto result = core::maps::MapsManager::loadFromFile(m_mapPath);
    if (!result) {
        LOG_E("Failed to load map: {}", result.error());
        goToMenu();
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

void GameScreen::handleEvent(const sf::Event &event) {
    if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            if (m_networked) m_network.disconnect();
            goToMenu();
        }
    }
}

void GameScreen::update(float dt) {
    if (m_networked) {
        // Send local input to server; simulation runs on the server side.
        if (m_localPlayer != entt::null) {
            core::protocol::PlayerInputPacket pkt{m_lastInput.tick, m_localPlayerId, m_lastInput.dir};
            m_network.sendInput(pkt);
        }
        return;
    }

    // Offline / standalone: run simulation locally.
    m_simulation.applyInput(m_registry, m_localPlayer, m_lastInput);
    m_simulation.update(m_registry, dt, m_map);
}

void GameScreen::draw(sf::RenderWindow &window) {
    m_renderer.render(window, m_registry, m_map);

    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("HUD", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

    if (m_localPlayer != entt::null) {
        auto *ps = m_registry.try_get<core::ecs::PlayerState>(m_localPlayer);
        if (ps) {
            ImGui::Text("Score: %d", ps->score);
            ImGui::Text("Lives: %d", ps->lives);
        }
    }

    ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
    ImGui::End();
}

void GameScreen::setLastInput(const core::ecs::Input &input) { m_lastInput = input; }

// ---------------------------------------------------------------------------
// ISubscriber<ClientNetworkEvent>
// ---------------------------------------------------------------------------

void GameScreen::onUpdate(const ClientNetworkEvent &event) {
    std::visit(
        pacman::overloaded{
            [this](const GameSnapshotEvent &e) { applySnapshot(e.packet); },
            [this](const RoundEndEvent &e) {
                LOG_I("Round ended — transitioning to ResultsScreen");
                m_screenManager.setScreen(std::make_unique<ResultsScreen>(
                    m_screenManager, &m_network, e.packet, m_localPlayerId, m_isHost,
                    m_mapPath, m_serverAddress, m_serverPort));
            },
            [this](const DisconnectedEvent &) {
                LOG_I("Disconnected during game");
                goToMenu();
            },
            [this](const ServerShutdownEvent &e) {
                LOG_I("Server shutdown during game: {}", e.packet.reason);
                goToMenu();
            },
            [](const auto &) {}},
        event);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void GameScreen::applySnapshot(const core::protocol::GameSnapshotPacket &snap) {
    for (int i = 0; i < snap.playerCount; ++i) {
        const auto &state = snap.players[i];
        auto it = m_playerEntities.find(state.id);
        if (it == m_playerEntities.end()) continue;
        entt::entity e = it->second;

        auto *pos = m_registry.try_get<core::ecs::Position>(e);
        if (pos) { pos->x = state.x; pos->y = state.y; }

        auto *ps = m_registry.try_get<core::ecs::PlayerState>(e);
        if (ps) { ps->score = state.score; ps->lives = state.lives; }

        auto *ds = m_registry.try_get<core::ecs::DirectionState>(e);
        if (ds) ds->current = state.dir;
    }

    for (int i = 0; i < core::ghostCount; ++i) {
        entt::entity e = m_ghostEntities[i];
        if (e == entt::null) continue;
        const auto &gs = snap.ghosts[i];

        auto *pos = m_registry.try_get<core::ecs::Position>(e);
        if (pos) { pos->x = gs.x; pos->y = gs.y; }

        auto *ghostState = m_registry.try_get<core::ecs::GhostState>(e);
        if (ghostState) ghostState->mode = static_cast<core::ecs::GhostState::Mode>(gs.mode);

        auto *ds = m_registry.try_get<core::ecs::DirectionState>(e);
        if (ds) ds->current = gs.dir;
    }
}

void GameScreen::spawnEntitiesFromMap() {
    float ts = m_map.tileSize;

    for (int row = 0; row < static_cast<int>(m_map.height); ++row) {
        for (int col = 0; col < static_cast<int>(m_map.width); ++col) {
            char tile = m_map.tileAt(col, row);
            float x = static_cast<float>(col) * ts;
            float y = static_cast<float>(row) * ts;

            if (tile == '#') {
                auto wall = m_registry.create();
                m_registry.emplace<core::ecs::Position>(wall, x, y);
                m_registry.emplace<core::ecs::Collider>(wall, ts, ts);
                m_registry.emplace<core::ecs::WallTag>(wall);
            } else if (tile == '.') {
                auto pellet = m_registry.create();
                m_registry.emplace<core::ecs::Position>(pellet, x, y);
                m_registry.emplace<core::ecs::PelletTag>(pellet);
            } else if (tile == 'o') {
                auto powerPellet = m_registry.create();
                m_registry.emplace<core::ecs::Position>(powerPellet, x, y);
                m_registry.emplace<core::ecs::PowerPelletTag>(powerPellet);
            }
        }
    }

    if (!m_map.pacmanSpawns.empty()) {
        const auto &spawn = m_map.pacmanSpawns[0];
        float x = static_cast<float>(spawn.col()) * ts;
        float y = static_cast<float>(spawn.row()) * ts;

        m_localPlayer = m_registry.create();
        m_registry.emplace<core::ecs::Position>(m_localPlayer, x, y);
        m_registry.emplace<core::ecs::Velocity>(m_localPlayer, core::defaultSpeed);
        m_registry.emplace<core::ecs::DirectionState>(m_localPlayer);
        m_registry.emplace<core::ecs::Collider>(m_localPlayer, ts * 0.9f, ts * 0.9f);
        m_registry.emplace<core::ecs::PlayerState>(m_localPlayer);
        m_registry.emplace<core::ecs::PacManTag>(m_localPlayer);
    }
}

void GameScreen::goToMenu() {
    m_screenManager.setScreen(
        std::make_unique<MenuScreen>(m_screenManager, m_network, m_mapPath, m_serverAddress, m_serverPort));
}

}  // namespace pacman::client::screens
