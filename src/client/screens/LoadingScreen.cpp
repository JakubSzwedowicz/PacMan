#include "client/screens/LoadingScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include "client/screen/ScreenManager.hpp"
#include "client/screens/GameScreen.hpp"
#include "client/screens/MenuScreen.hpp"
#include "core/Common.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"

namespace pacman::client::screens {

using namespace network::events;

LoadingScreen::LoadingScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network,
                             std::string mapPath, std::string serverAddress, int serverPort,
                             core::protocol::GameStartPacket gameStart, core::PlayerId localPlayerId,
                             bool isHost)
    : m_screenManager(screenManager),
      m_network(network),
      m_mapPath(std::move(mapPath)),
      m_serverAddress(std::move(serverAddress)),
      m_serverPort(serverPort),
      m_gameStart(std::move(gameStart)),
      m_localPlayerId(localPlayerId),
      m_isHost(isHost) {
    m_ghostEntities.fill(entt::null);
}

void LoadingScreen::onEnter() { LOG_I("LoadingScreen entered (playerId={})", m_localPlayerId); }
void LoadingScreen::onExit() { LOG_I("LoadingScreen exited"); }

void LoadingScreen::handleEvent(const sf::Event &) {}

void LoadingScreen::update(float /*dt*/) {
    if (!m_mapParsed) {
        auto result = core::maps::MapsManager::loadFromJson(m_gameStart.mapJson);
        if (!result) {
            LOG_E("Failed to parse map JSON: {}", result.error());
            goToMenu();
            return;
        }
        m_map = std::move(*result);
        m_mapParsed = true;
        LOG_I("Map '{}' parsed ({}x{})", m_map.name, m_map.width, m_map.height);
        return;
    }
    if (!m_assetsLoaded) {
        // Asset loading (textures/audio) will be added when the Renderer is extended.
        m_assetsLoaded = true;
        LOG_I("Assets loaded");
        return;
    }
    if (!m_simInitialized) {
        spawnEntitiesFromMap();
        m_simInitialized = true;
        LOG_I("ECS initialised ({} entities)", m_registry.storage<entt::entity>().size());
        return;
    }
    if (!m_readyToPlaySent) {
        m_network.sendReadyToPlay();
        m_readyToPlaySent = true;
        LOG_I("ReadyToPlay sent — waiting for server");
    }
}

void LoadingScreen::draw(sf::RenderWindow & /*window*/) {
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);
    ImGui::Begin("Ładowanie...", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("%s Wczytywanie danych mapy", m_mapParsed ? "[X]" : "[ ]");
    ImGui::Text("%s Ładowanie zasobów", m_assetsLoaded ? "[X]" : "[ ]");
    ImGui::Text("%s Inicjalizacja symulacji", m_simInitialized ? "[X]" : "[ ]");
    ImGui::Text("%s Oczekiwanie na graczy...", m_readyToPlaySent ? "[X]" : "[ ]");

    ImGui::End();
}

void LoadingScreen::onUpdate(const ClientNetworkEvent &event) {
    std::visit(
        pacman::overloaded{
            [this](const GameSnapshotEvent &) {
                LOG_I("First snapshot received — transitioning to GameScreen");
                m_screenManager.setScreen(std::make_unique<GameScreen>(
                    m_screenManager, m_network, std::move(m_registry), std::move(m_map),
                    std::move(m_playerEntities), m_ghostEntities, m_localPlayerId, m_isHost,
                    m_mapPath, m_serverAddress, m_serverPort));
            },
            [this](const DisconnectedEvent &) {
                LOG_I("Disconnected during loading");
                goToMenu();
            },
            [this](const ServerShutdownEvent &e) {
                LOG_I("Server shutdown during loading: {}", e.packet.reason);
                goToMenu();
            },
            [](const auto &) {}},
        event);
}

void LoadingScreen::spawnEntitiesFromMap() {
    const float ts = m_map.tileSize;

    // Tiles: walls, pellets, power pellets
    for (int row = 0; row < static_cast<int>(m_map.height); ++row) {
        for (int col = 0; col < static_cast<int>(m_map.width); ++col) {
            char tile = m_map.tileAt(col, row);
            float x = static_cast<float>(col) * ts;
            float y = static_cast<float>(row) * ts;

            if (tile == '#') {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, x, y);
                m_registry.emplace<core::ecs::Collider>(e, ts, ts);
                m_registry.emplace<core::ecs::WallTag>(e);
            } else if (tile == '.') {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, x, y);
                m_registry.emplace<core::ecs::PelletTag>(e);
            } else if (tile == 'o') {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, x, y);
                m_registry.emplace<core::ecs::PowerPelletTag>(e);
            }
        }
    }

    // Players: one entity per assigned player in the GameStart packet
    for (int i = 0; i < m_gameStart.playerCount; ++i) {
        const core::PlayerId pid = m_gameStart.playerIds[i];
        const auto &spawn = m_gameStart.spawnPositions[i];
        float x = static_cast<float>(spawn.col()) * ts;
        float y = static_cast<float>(spawn.row()) * ts;

        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, x, y);
        m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts * 0.9f, ts * 0.9f);
        m_registry.emplace<core::ecs::PlayerState>(e);
        m_registry.emplace<core::ecs::PacManTag>(e);
        m_playerEntities[pid] = e;
    }

    // Ghosts: spawn at map ghost spawn positions
    const std::array<core::maps::Tile, core::ghostCount> ghostTiles = {
        m_map.ghostSpawns.blinky, m_map.ghostSpawns.pinky,
        m_map.ghostSpawns.inky,   m_map.ghostSpawns.clyde};

    const std::array<core::ecs::GhostType, core::ghostCount> ghostTypes = {
        core::ecs::GhostType::Blinky, core::ecs::GhostType::Pinky,
        core::ecs::GhostType::Inky,   core::ecs::GhostType::Clyde};

    for (int i = 0; i < core::ghostCount; ++i) {
        const auto &tile = ghostTiles[i];
        if (tile.col() == 0 && tile.row() == 0) {
            m_ghostEntities[i] = entt::null;
            continue;
        }
        float x = static_cast<float>(tile.col()) * ts;
        float y = static_cast<float>(tile.row()) * ts;

        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, x, y);
        m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed * 0.75f);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts * 0.9f, ts * 0.9f);
        m_registry.emplace<core::ecs::GhostState>(e, core::ecs::GhostState::Mode::Scatter, ghostTypes[i]);
        m_registry.emplace<core::ecs::GhostTag>(e);
        m_ghostEntities[i] = e;
    }
}

void LoadingScreen::goToMenu() {
    m_screenManager.setScreen(
        std::make_unique<MenuScreen>(m_screenManager, m_network, m_mapPath, m_serverAddress, m_serverPort));
}

}  // namespace pacman::client::screens
