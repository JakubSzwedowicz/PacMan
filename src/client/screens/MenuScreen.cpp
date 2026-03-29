#include "client/screens/MenuScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include "client/screen/ScreenManager.hpp"
#include "client/screens/GameScreen.hpp"

namespace pacman::client::screens {

MenuScreen::MenuScreen(screen::ScreenManager &screenManager, std::string mapPath)
    : m_screenManager(screenManager), m_mapPath(std::move(mapPath)) {}

void MenuScreen::onEnter() { LOG_I("MenuScreen entered"); }

void MenuScreen::onExit() { LOG_I("MenuScreen exited"); }

void MenuScreen::handleEvent(const sf::Event &event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto *key = event.getIf<sf::Event::KeyPressed>();
        if (key && key->code == sf::Keyboard::Key::Escape) {
            m_shouldQuit = true;
        }
    }
}

void MenuScreen::update(float /*dt*/) {}

void MenuScreen::draw(sf::RenderWindow & /*window*/) {
    ImGui::SetNextWindowPos(ImVec2(200, 150), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("PacMan", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("PacMan").x) / 2.0f);
    ImGui::Text("PacMan");
    ImGui::Separator();
    ImGui::Spacing();

    float buttonWidth = 200.0f;
    float buttonX = (400 - buttonWidth) / 2.0f;

    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Host Game", ImVec2(buttonWidth, 40))) {
        LOG_I("Host Game clicked");
        m_screenManager.setScreen(std::make_unique<GameScreen>(m_screenManager, m_mapPath));
    }

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Join Game", ImVec2(buttonWidth, 40))) {
        LOG_I("Join Game clicked");
        m_screenManager.setScreen(std::make_unique<GameScreen>(m_screenManager, m_mapPath));
    }

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Quit", ImVec2(buttonWidth, 40))) {
        m_shouldQuit = true;
    }

    ImGui::End();
}

bool MenuScreen::shouldQuit() const { return m_shouldQuit; }

}  // namespace pacman::client::screens
