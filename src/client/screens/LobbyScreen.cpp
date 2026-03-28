#include "client/screens/LobbyScreen.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/screens/LoadingScreen.hpp"
#include "client/screens/MenuScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <imgui.h>

namespace pacman::client::screens {

LobbyScreen::LobbyScreen(screen::ScreenManager &screenManager,
                         network::ClientNetwork &network,
                         core::PlayerId localPlayerId, bool isHost)
    : m_screenManager(screenManager), m_network(network),
      m_localPlayerId(localPlayerId), m_isHost(isHost) {}

void LobbyScreen::onEnter() {
  LOG_I("LobbyScreen entered");
  m_network.setListener(this);
}

void LobbyScreen::onExit() {
  LOG_I("LobbyScreen exited");
  m_network.setListener(nullptr);
}

void LobbyScreen::handleEvent(const sf::Event &event) {
  if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
    if (key->code == sf::Keyboard::Key::Escape) {
      m_network.disconnect();
      // TODO: m_screenManager.setScreen<MenuScreen>(...)
    }
  }
}

void LobbyScreen::update(float /*dt*/) { m_network.poll(); }

void LobbyScreen::draw(sf::RenderWindow & /*window*/) {
  ImGui::SetNextWindowPos(ImVec2(150, 100), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  ImGui::Begin("Lobby");

  ImGui::BeginTable("Players", 2, ImGuiTableFlags_Borders);
  ImGui::TableSetupColumn("Gracz");
  ImGui::TableSetupColumn("Status");
  ImGui::TableHeadersRow();
  for (int i = 0; i < m_lobbyState.playerCount; ++i) {
    const auto &p = m_lobbyState.players[i];
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s%s", p.name.c_str(), p.id == 1 ? " (Host)" : "");
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", p.ready ? "Gotowy" : "Czeka");
  }
  ImGui::EndTable();

  ImGui::Spacing();
  const char *label = m_localReady ? "Nie gotowy" : "Gotowy";
  if (ImGui::Button(label)) {
    m_localReady = !m_localReady;
    m_network.sendLobbyReady(m_localReady);
  }

  if (m_isHost) {
    ImGui::SameLine();
    // Host sends ready=true as the start signal (LobbyPhase triggers game)
    if (ImGui::Button("Rozpocznij") && m_localReady) {
      m_network.sendLobbyReady(true);
    }
  }

  ImGui::End();
}

void LobbyScreen::onConnected(core::PlayerId /*assignedId*/) {}
void LobbyScreen::onDisconnected() {
  // TODO: m_screenManager.setScreen<MenuScreen>(...)
}
void LobbyScreen::onLobbyState(const core::protocol::LobbyStatePacket &pkt) {
  m_lobbyState = pkt;
}
void LobbyScreen::onGameStart(const core::protocol::GameStartPacket &pkt) {
  LOG_I("GameStart received — transitioning to LoadingScreen");
  m_screenManager.setScreen(std::make_unique<LoadingScreen>(
      m_screenManager, m_network, pkt, m_localPlayerId));
}
void LobbyScreen::onGameSnapshot(const core::protocol::GameSnapshotPacket &) {}
void LobbyScreen::onRoundEnd(const core::protocol::RoundEndPacket &) {}
void LobbyScreen::onServerShutdown(
    const core::protocol::ServerShutdownPacket &pkt) {
  LOG_I("Server shutdown: {}", pkt.reason);
  // TODO: setScreen<MenuScreen>
}

} // namespace pacman::client::screens
