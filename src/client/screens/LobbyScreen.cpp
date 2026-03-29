#include "client/screens/LobbyScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include "client/screen/ScreenManager.hpp"
#include "client/screens/LoadingScreen.hpp"
#include "client/screens/MenuScreen.hpp"
#include "core/Common.hpp"

namespace pacman::client::screens {

using namespace network::events;

LobbyScreen::LobbyScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network,
                         core::PlayerId localPlayerId, bool isHost)
    : m_screenManager(screenManager), m_network(network), m_localPlayerId(localPlayerId), m_isHost(isHost) {}

void LobbyScreen::onEnter() { LOG_I("LobbyScreen entered"); }
void LobbyScreen::onExit() { LOG_I("LobbyScreen exited"); }

void LobbyScreen::handleEvent(const sf::Event &event) {
    if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            m_network.disconnect();
            // TODO: m_screenManager.setScreen<MenuScreen>(...)
        }
    }
}

void LobbyScreen::update(float /*dt*/) {}

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
                                      m_screenManager.setScreen(std::make_unique<LoadingScreen>(
                                          m_screenManager, m_network, e.packet, m_localPlayerId));
                                  },
                                  [this](const DisconnectedEvent &) {
                                      LOG_I("Disconnected — returning to MenuScreen");
                                      // TODO: m_screenManager.setScreen<MenuScreen>(...)
                                  },
                                  [this](const ServerShutdownEvent &e) {
                                      LOG_I("Server shutdown: {}", e.packet.reason);
                                      // TODO: m_screenManager.setScreen<MenuScreen>(...)
                                  },
                                  [](const auto &) {}},
               event);
}

}  // namespace pacman::client::screens
