#include "client/graphics/AppWindow.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <SFML/Graphics/View.hpp>
#include <imgui-SFML.h>

namespace pacman::client::graphics {

AppWindow::AppWindow(const std::string& title, unsigned int width, unsigned int height)
    : m_window(sf::VideoMode({width, height}), title) {
    m_imguiInitialized = ImGui::SFML::Init(m_window);
    LOG_I("AppWindow created: {}x{}", width, height);
    if (!m_imguiInitialized) {
        LOG_E("Failed to initialize ImGui-SFML");
    }
}

AppWindow::~AppWindow() { shutdown(); }

void AppWindow::pollEvents(const std::function<void(const sf::Event&)>& callback) {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            continue;
        }
        if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            m_window.setView(sf::View(sf::FloatRect(
                sf::Vector2f(0.0f, 0.0f),
                sf::Vector2f(static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)))));
        }
        if (m_imguiInitialized) {
            ImGui::SFML::ProcessEvent(m_window, *event);
        }
        callback(*event);
    }
}

void AppWindow::renderFrame(sf::Time dt, const std::function<void(sf::RenderWindow&)>& drawFn) {
    if (m_imguiInitialized) {
        ImGui::SFML::Update(m_window, dt);
    }
    m_window.clear(sf::Color::Black);
    drawFn(m_window);
    if (m_imguiInitialized) {
        ImGui::SFML::Render(m_window);
    }
    m_window.display();
}

bool AppWindow::isOpen() const { return m_window.isOpen(); }

void AppWindow::shutdown() {
    if (!m_imguiInitialized) return;
    // Close the window first so SFML doesn't try to restore ImGui's X11 cursor
    // after ImGui::SFML::Shutdown() has already freed it (BadCursor X error).
    if (m_window.isOpen()) m_window.close();
    ImGui::SFML::Shutdown();
    m_imguiInitialized = false;
    LOG_I("AppWindow shut down");
}

}  // namespace pacman::client::graphics
