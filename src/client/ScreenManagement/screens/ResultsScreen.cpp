#include "client/ScreenManagement/screens/ResultsScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include <algorithm>

namespace pacman::client::screens {

ResultsScreen::ResultsScreen(network::ClientNetwork *network, core::protocol::RoundEndPacket results)
    : m_network(network), m_results(std::move(results)) {}

void ResultsScreen::onEnter() { LOG_I("ResultsScreen entered"); }
void ResultsScreen::onExit() { LOG_I("ResultsScreen exited"); }
screen::ScreenRequest ResultsScreen::update(float /*dt*/, const input::InputSnapshot &input) {
    if (input.escapePressed) {
        if (m_network) m_network->disconnect();
        queueRequest(screen::OpenMenuRequest{});
    }
    return takeQueuedRequest();
}

void ResultsScreen::draw(sf::RenderWindow &window) {
    const auto size = window.getSize();
    const float panelWidth = std::min(500.0f, std::max(360.0f, static_cast<float>(size.x) - 32.0f));
    const float panelHeight = std::min(400.0f, std::max(280.0f, static_cast<float>(size.y) - 32.0f));

    ImGui::SetNextWindowPos(ImVec2((static_cast<float>(size.x) - panelWidth) * 0.5f,
                                   (static_cast<float>(size.y) - panelHeight) * 0.5f),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
    ImGui::Begin("Koniec rundy");

    // Sort players by score descending for display
    auto scores = m_results.finalScores;
    std::sort(scores.begin(), scores.end(), [](const auto &a, const auto &b) { return a.score > b.score; });

    ImGui::BeginTable("Results", 3, ImGuiTableFlags_Borders);
    ImGui::TableSetupColumn("Miejsce");
    ImGui::TableSetupColumn("Gracz");
    ImGui::TableSetupColumn("Wynik");
    ImGui::TableHeadersRow();
    for (size_t i = 0; i < scores.size(); ++i) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%zu.", i + 1);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", scores[i].name.empty() ? ("Gracz " + std::to_string(scores[i].id)).c_str()
                                                 : scores[i].name.c_str());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d pkt", scores[i].score);
    }
    ImGui::EndTable();

    ImGui::Spacing();
    if (ImGui::Button("Wróć do menu")) {
        if (m_network) m_network->disconnect();
        queueRequest(screen::OpenMenuRequest{});
    }

    ImGui::End();
}

}  // namespace pacman::client::screens
