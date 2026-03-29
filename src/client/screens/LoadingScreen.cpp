#include "client/screens/LoadingScreen.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/screens/MenuScreen.hpp"

#include "core/Common.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <imgui.h>

namespace pacman::client::screens {

using namespace network::events;

LoadingScreen::LoadingScreen(screen::ScreenManager &screenManager,
                             network::ClientNetwork &network,
                             core::protocol::GameStartPacket gameStart,
                             core::PlayerId localPlayerId)
    : m_screenManager(screenManager), m_network(network),
      m_gameStart(std::move(gameStart)), m_localPlayerId(localPlayerId) {}

void LoadingScreen::onEnter() { LOG_I("LoadingScreen entered"); }
void LoadingScreen::onExit()  { LOG_I("LoadingScreen exited"); }

void LoadingScreen::handleEvent(const sf::Event &) {}

void LoadingScreen::update(float /*dt*/) {
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

  ImGui::Text("%s Wczytywanie danych mapy",  m_mapParsed      ? "[X]" : "[ ]");
  ImGui::Text("%s Ładowanie zasobów",        m_assetsLoaded   ? "[X]" : "[ ]");
  ImGui::Text("%s Inicjalizacja symulacji",  m_simInitialized ? "[X]" : "[ ]");
  ImGui::Text("%s Oczekiwanie na graczy...", m_readyToPlaySent ? "[X]" : "[ ]");

  ImGui::End();
}

void LoadingScreen::onUpdate(const ClientNetworkEvent &event) {
  std::visit(pacman::overloaded{
    [this](const GameSnapshotEvent &) {
        LOG_I("First snapshot received — transitioning to GameScreen");
        // TODO: m_screenManager.setScreen<GameScreen>(registry, map, network, ...)
    },
    [this](const DisconnectedEvent &) {
        LOG_I("Disconnected during loading");
        // TODO: m_screenManager.setScreen<MenuScreen>(...)
    },
    [this](const ServerShutdownEvent &e) {
        LOG_I("Server shutdown during loading: {}", e.packet.reason);
        // TODO: m_screenManager.setScreen<MenuScreen>(...)
    },
    [](const auto &) {}
  }, event);
}

} // namespace pacman::client::screens
