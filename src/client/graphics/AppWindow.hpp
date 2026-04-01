#pragma once

#include <Utils/Logging/Logger.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <string>

namespace pacman::client::graphics {

// Owns the SFML render window and the ImGui-SFML overlay.
class AppWindow {
   public:
    AppWindow(const std::string& title, unsigned int width, unsigned int height);
    ~AppWindow();

    AppWindow(const AppWindow&) = delete;
    AppWindow& operator=(const AppWindow&) = delete;
    AppWindow(AppWindow&&) = delete;
    AppWindow& operator=(AppWindow&&) = delete;

    void pollEvents(const std::function<void(const sf::Event&)>& callback);
    void renderFrame(sf::Time dt, const std::function<void(sf::RenderWindow&)>& drawFn);

    [[nodiscard]] bool isOpen() const;

    void shutdown();

   private:
    sf::RenderWindow m_window;
    bool m_imguiInitialized = false;
    Utils::Logging::Logger m_logger{"AppWindow"};
};

}  // namespace pacman::client::graphics
