#include "client/ui/UIOverlay.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui-SFML.h>

namespace pacman::client::ui {

UIOverlay::UIOverlay() { LOG_I("UIOverlay created"); }

UIOverlay::~UIOverlay() {
    if (m_initialized) {
        shutdown();
    }
}

void UIOverlay::init(sf::RenderWindow &window) {
    if (m_initialized) {
        return;
    }
    [[maybe_unused]] bool ok = ImGui::SFML::Init(window);
    m_initialized = true;
    LOG_I("ImGui-SFML initialized");
}

void UIOverlay::processEvent(const sf::RenderWindow &window, const sf::Event &event) {
    if (m_initialized) {
        ImGui::SFML::ProcessEvent(window, event);
    }
}

void UIOverlay::update(sf::RenderWindow &window, sf::Time dt) {
    if (m_initialized) {
        ImGui::SFML::Update(window, dt);
    }
}

void UIOverlay::render(sf::RenderWindow &window) {
    if (m_initialized) {
        ImGui::SFML::Render(window);
    }
}

void UIOverlay::shutdown() {
    if (m_initialized) {
        ImGui::SFML::Shutdown();
        m_initialized = false;
        LOG_I("ImGui-SFML shut down");
    }
}

}  // namespace pacman::client::ui
