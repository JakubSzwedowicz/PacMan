#include "client/ScreenManagement/screens/LoadingScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include "core/Common.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"

namespace pacman::client::screens {

using namespace network::events;

LoadingScreen::LoadingScreen(network::ClientNetwork &network, core::protocol::GameStartPacket gameStart,
                             core::PlayerId localPlayerId, bool isHost)
    : m_network(network), m_gameStart(std::move(gameStart)), m_localPlayerId(localPlayerId), m_isHost(isHost) {
    m_ghostEntities.fill(entt::null);
}

void LoadingScreen::onEnter() { LOG_I("LoadingScreen entered (playerId={})", m_localPlayerId); }
void LoadingScreen::onExit() { LOG_I("LoadingScreen exited"); }

screen::ScreenRequest LoadingScreen::update(float /*dt*/, const input::InputSnapshot &input) {
    if (input.escapePressed) {
        m_network.disconnect();
        return screen::OpenMenuRequest{};
    }
    if (!m_mapParsed) {
        auto result = core::maps::MapsManager::loadFromJson(m_gameStart.mapJson);
        if (!result) {
            LOG_E("Failed to parse map JSON: {}", result.error());
            queueRequest(screen::OpenMenuRequest{});
            return takeQueuedRequest();
        }
        m_map = std::move(*result);
        m_mapParsed = true;
        LOG_I("Map '{}' parsed ({}x{})", m_map.name, m_map.width, m_map.height);
        return takeQueuedRequest();
    }
    if (!m_assetsLoaded) {
        // Asset loading (textures/audio) will be added when the Renderer is extended.
        m_assetsLoaded = true;
        LOG_I("Assets loaded");
        return takeQueuedRequest();
    }
    if (!m_simInitialized) {
        spawnEntitiesFromMap();
        m_simInitialized = true;
        LOG_I("ECS initialised ({} entities)", m_registry.storage<entt::entity>().size());
        return takeQueuedRequest();
    }
    if (!m_readyToPlaySent) {
        m_network.sendReadyToPlay();
        m_readyToPlaySent = true;
        LOG_I("ReadyToPlay sent — waiting for server");
        return takeQueuedRequest();
    }
    if (m_firstSnapshotReceived) {
        LOG_I("First snapshot received — transitioning to GameScreen");
        queueRequest(screen::OpenNetworkGameRequest{
            std::move(m_registry),
            std::move(m_map),
            std::move(m_playerEntities),
            m_ghostEntities,
            m_localPlayerId,
            m_isHost,
        });
    }

    return takeQueuedRequest();
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
    std::visit(pacman::overloaded{[this](const GameSnapshotEvent &) { m_firstSnapshotReceived = true; },
                                  [this](const DisconnectedEvent &) {
                                      LOG_I("Disconnected during loading");
                                      queueRequest(screen::OpenMenuRequest{});
                                  },
                                  [this](const ServerShutdownEvent &e) {
                                      LOG_I("Server shutdown during loading: {}", e.packet.reason);
                                      queueRequest(screen::OpenMenuRequest{});
                                  },
                                  [](const auto &) {}},
               event);
}

void LoadingScreen::spawnEntitiesFromMap() {
    const float ts = m_map.tileSize;

    // Tiles: walls, pellets, power pellets
    for (core::maps::Tile::Unit row = 0; row < m_map.height; ++row) {
        for (core::maps::Tile::Unit col = 0; col < m_map.width; ++col) {
            const auto tileType = m_map.tileTypeAt(col, row);
            const float x = static_cast<float>(col) * ts;
            const float y = static_cast<float>(row) * ts;

            if (tileType == core::maps::TileType::Wall) {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, x, y);
                m_registry.emplace<core::ecs::Collider>(e, ts, ts);
                m_registry.emplace<core::ecs::WallTag>(e);
            } else if (tileType == core::maps::TileType::Pellet) {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, x, y);
                m_registry.emplace<core::ecs::PelletTag>(e);
            } else if (tileType == core::maps::TileType::PowerPellet) {
                auto e = m_registry.create();
                m_registry.emplace<core::ecs::Position>(e, x, y);
                m_registry.emplace<core::ecs::PowerPelletTag>(e);
            }
        }
    }

    // Players: one entity per assigned player in the GameStart packet
    for (size_t i = 0; i < m_gameStart.playerIds.size(); ++i) {
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
        m_map.ghostSpawns.blinky, m_map.ghostSpawns.pinky, m_map.ghostSpawns.inky, m_map.ghostSpawns.clyde};

    const std::array<core::ecs::GhostType, core::ghostCount> ghostTypes = {
        core::ecs::GhostType::Blinky, core::ecs::GhostType::Pinky, core::ecs::GhostType::Inky,
        core::ecs::GhostType::Clyde};

    const bool hasGhostHouse = !(m_map.ghostHouseExit.col() == 0 && m_map.ghostHouseExit.row() == 0);
    const auto initialGhostMode =
        hasGhostHouse ? core::ecs::GhostState::Mode::InHouse : core::ecs::GhostState::Mode::Scatter;

    for (int i = 0; i < core::ghostCount; ++i) {
        const auto &tile = ghostTiles[i];
        if (tile.col() == 0 && tile.row() == 0) {
            m_ghostEntities[i] = entt::null;
            continue;
        }
        const float x = static_cast<float>(tile.col()) * ts;
        const float y = static_cast<float>(tile.row()) * ts;

        auto e = m_registry.create();
        m_registry.emplace<core::ecs::Position>(e, x, y);
        m_registry.emplace<core::ecs::Velocity>(e, core::defaultSpeed * 0.75f);
        m_registry.emplace<core::ecs::DirectionState>(e);
        m_registry.emplace<core::ecs::Collider>(e, ts * 0.9f, ts * 0.9f);
        m_registry.emplace<core::ecs::GhostState>(e, initialGhostMode, ghostTypes[i]);
        m_registry.emplace<core::ecs::GhostTag>(e);
        m_ghostEntities[i] = e;
    }
}

}  // namespace pacman::client::screens
