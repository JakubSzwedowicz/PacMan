#include "client/screens/LoadingScreen.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/screens/MenuScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <imgui.h>

namespace pacman::client::screens {

LoadingScreen::LoadingScreen(
    screen::ScreenManager &screenManager, network::ClientNetwork &network,
    core::protocol::GameStartPacket gameStart, core::PlayerId localPlayerId,
    std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_screenManager(screenManager), m_network(network),
      m_gameStart(std::move(gameStart)), m_localPlayerId(localPlayerId),
      m_loggerConfig(std::move(loggerConfig)),
      m_logger("LoadingScreen", m_loggerConfig) {}

void LoadingScreen::onEnter() {
  LOG_I("LoadingScreen entered");
  m_network.setListener(this);
}

void LoadingScreen::onExit() {
  LOG_I("LoadingScreen exited");
  m_network.setListener(nullptr);
}

void LoadingScreen::handleEvent(const sf::Event &) {}

void LoadingScreen::update(float /*dt*/) {
  m_network.poll();

  // Tick through loading steps one per update()
  if (!m_mapParsed) {
    // TODO: parse m_gameStart.mapJson via MapsManager::loadFromJson
    m_mapParsed = true;
    LOG_I("Map parsed");
    return;
  }
  if (!m_assetsLoaded) {
    // TODO: load textures / audio
    m_assetsLoaded = true;
    LOG_I("Assets loaded");
    return;
  }
  if (!m_simInitialized) {
    // TODO: init local ECS registry from map + spawn positions
    m_simInitialized = true;
    LOG_I("Simulation initialised");
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

void LoadingScreen::onDisconnected() {
  LOG_I("Disconnected during loading");
  // TODO: setScreen<MenuScreen>
}

void LoadingScreen::onGameSnapshot(
    const core::protocol::GameSnapshotPacket & /*pkt*/) {
  // Server sends the first snapshot when all players are ready.
  // Transition to GameScreen now.
  LOG_I("First snapshot received — transitioning to GameScreen");
  // TODO: m_screenManager.setScreen<GameScreen>(registry, map, network, ...)
}

void LoadingScreen::onServerShutdown(
    const core::protocol::ServerShutdownPacket &pkt) {
  LOG_I("Server shutdown during loading: {}", pkt.reason);
  // TODO: setScreen<MenuScreen>
}

} // namespace pacman::client::screens
