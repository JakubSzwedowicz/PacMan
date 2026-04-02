#include "client/ScreenManagement/screens/LobbyScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include "core/Common.hpp"

namespace pacman::client::screens {

using namespace network::events;

LobbyScreen::LobbyScreen(network::ClientNetwork &network, core::PlayerId localPlayerId, bool isHost)
    : m_network(network), m_localPlayerId(localPlayerId), m_isHost(isHost) {}

void LobbyScreen::onEnter() { LOG_I("LobbyScreen entered (host={})", m_isHost); }
void LobbyScreen::onExit() { LOG_I("LobbyScreen exited"); }

screen::ScreenRequest LobbyScreen::update(float /*dt*/, const input::InputSnapshot &input) {
    if (input.escapePressed) {
        m_network.disconnect();
        queueRequest(screen::OpenMenuRequest{});
    }
    return takeQueuedRequest();
}

void LobbyScreen::draw(sf::RenderWindow & /*window*/) {
    ImGui::SetNextWindowPos(ImVec2(150, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Lobby");

    ImGui::BeginTable("Players", 2, ImGuiTableFlags_Borders);
    ImGui::TableSetupColumn("Gracz");
    ImGui::TableSetupColumn("Status");
    ImGui::TableHeadersRow();
    for (size_t i = 0; i < m_lobbyState.players.size(); ++i) {
        const auto &p = m_lobbyState.players[i];
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s%s", p.name.c_str(), i == 0 ? " (Host)" : "");
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
        if (ImGui::Button("Rozpocznij") && m_localReady) {
            m_network.sendLobbyReady(true);
        }
    }

    ImGui::End();
}

void LobbyScreen::onUpdate(const ClientNetworkEvent &event) {
    std::visit(pacman::overloaded{[this](const LobbyStateEvent &e) { m_lobbyState = e.packet; },
                                  [this](const GameStartEvent &e) {
                                      LOG_I("GameStart received — transitioning to LoadingScreen");
                                      queueRequest(screen::OpenLoadingRequest{
                                          e.packet,
                                          e.packet.assignedPlayerId,
                                          m_isHost,
                                      });
                                  },
                                  [this](const DisconnectedEvent &) {
                                      LOG_I("Disconnected — returning to menu");
                                      queueRequest(screen::OpenMenuRequest{});
                                  },
                                  [this](const ServerShutdownEvent &e) {
                                      LOG_I("Server shutdown: {} — returning to menu", e.packet.reason);
                                      queueRequest(screen::OpenMenuRequest{});
                                  },
                                  [](const auto &) {}},
               event);
}

}  // namespace pacman::client::screens
