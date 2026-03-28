#include "client/screens/ResultsScreen.hpp"
#include "client/screen/ScreenManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <imgui.h>

#include <algorithm>

namespace pacman::client::screens {

ResultsScreen::ResultsScreen(screen::ScreenManager &screenManager,
                             network::ClientNetwork *network,
                             core::protocol::RoundEndPacket results,
                             core::PlayerId localPlayerId, bool isHost)
    : m_screenManager(screenManager), m_network(network),
      m_results(std::move(results)), m_localPlayerId(localPlayerId),
      m_isHost(isHost) {}

void ResultsScreen::onEnter() { LOG_I("ResultsScreen entered"); }
void ResultsScreen::onExit() { LOG_I("ResultsScreen exited"); }
void ResultsScreen::handleEvent(const sf::Event &) {}
void ResultsScreen::update(float /*dt*/) {}

void ResultsScreen::draw(sf::RenderWindow & /*window*/) {
  ImGui::SetNextWindowPos(ImVec2(150, 100), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  ImGui::Begin("Koniec rundy");

  // Sort players by score descending for display
  auto scores = m_results.finalScores;
  std::sort(scores.begin(), scores.begin() + m_results.playerCount,
            [](const auto &a, const auto &b) { return a.score > b.score; });

  ImGui::BeginTable("Results", 3, ImGuiTableFlags_Borders);
  ImGui::TableSetupColumn("Miejsce");
  ImGui::TableSetupColumn("Gracz");
  ImGui::TableSetupColumn("Wynik");
  ImGui::TableHeadersRow();
  for (int i = 0; i < m_results.playerCount; ++i) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%d.", i + 1);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("Gracz %u", scores[i].id);
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%d pkt", scores[i].score);
  }
  ImGui::EndTable();

  ImGui::Spacing();
  if (ImGui::Button("Wróć do menu")) {
    if (m_network)
      m_network->disconnect();
    // TODO: setScreen<MenuScreen>
  }

  if (m_isHost) {
    ImGui::SameLine();
    if (ImGui::Button("Następna runda")) {
      // TODO: setScreen<LobbyScreen> (reuse existing connection)
    }
  }

  ImGui::End();
}

} // namespace pacman::client::screens
