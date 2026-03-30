#include "client/screens/ResultsScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include <algorithm>

#include "client/screen/ScreenManager.hpp"
#include "client/screens/LobbyScreen.hpp"
#include "client/screens/MenuScreen.hpp"

namespace pacman::client::screens {

ResultsScreen::ResultsScreen(screen::ScreenManager &screenManager, network::ClientNetwork *network,
                             core::protocol::RoundEndPacket results, core::PlayerId localPlayerId,
                             bool isHost, std::string mapPath, std::string serverAddress, int serverPort)
    : m_screenManager(screenManager),
      m_network(network),
      m_results(std::move(results)),
      m_localPlayerId(localPlayerId),
      m_isHost(isHost),
      m_mapPath(std::move(mapPath)),
      m_serverAddress(std::move(serverAddress)),
      m_serverPort(serverPort) {}

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
        goToMenu();
    }

    if (m_isHost) {
        ImGui::SameLine();
        if (ImGui::Button("Następna runda")) {
            goToLobby();
        }
    }

    ImGui::End();
}

void ResultsScreen::goToMenu() {
    if (!m_network) return;
    m_network->disconnect();
    m_screenManager.setScreen(
        std::make_unique<MenuScreen>(m_screenManager, *m_network, m_mapPath, m_serverAddress, m_serverPort));
}

void ResultsScreen::goToLobby() {
    if (!m_network) return;
    // Connection stays alive — host goes back to lobby to start another round.
    m_screenManager.setScreen(std::make_unique<LobbyScreen>(m_screenManager, *m_network, m_mapPath,
                                                             m_serverAddress, m_serverPort,
                                                             m_localPlayerId, m_isHost));
}

}  // namespace pacman::client::screens
