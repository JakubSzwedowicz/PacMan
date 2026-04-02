#include "client/ScreenManagement/screens/LobbyScreen.hpp"

#include <algorithm>
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

void LobbyScreen::draw(sf::RenderWindow &window) {
    const auto size = window.getSize();
    const float panelWidth = std::min(500.0f, std::max(360.0f, static_cast<float>(size.x) - 32.0f));
    const float panelHeight = std::min(400.0f, std::max(280.0f, static_cast<float>(size.y) - 32.0f));

    ImGui::SetNextWindowPos(ImVec2((static_cast<float>(size.x) - panelWidth) * 0.5f,
                                   (static_cast<float>(size.y) - panelHeight) * 0.5f),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
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
    if (m_isHost) {
        if (ImGui::Button("Rozpocznij")) {
            m_network.sendLobbyReady(true);
        }
    } else {
        const char *label = m_localReady ? "Nie gotowy" : "Gotowy";
        if (ImGui::Button(label)) {
            m_localReady = !m_localReady;
            m_network.sendLobbyReady(m_localReady);
        }
    }

    ImGui::End();
}

void LobbyScreen::onUpdate(const ClientNetworkEvent &event) {
    std::visit(pacman::overloaded{[this](const LobbyStateEvent &e) {
                                      // If we haven't yet determined host status and we're the only
                                      // player in the first state we receive, we are the host
                                      // (handles the case where the server was started from terminal).
                                      if (!m_hostDetermined) {
                                          if (!m_isHost && e.packet.players.size() == 1) m_isHost = true;
                                          m_hostDetermined = true;
                                      }
                                      m_lobbyState = e.packet;
                                  },
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
