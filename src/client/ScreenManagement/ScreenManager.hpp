#pragma once

#include <Utils/Logging/Logger.h>

#include <functional>
#include <memory>

#include "client/input/InputSnapshot.hpp"
#include "client/ScreenManagement/Screen.hpp"

namespace pacman::client::screen {

class ScreenManager {
   public:
    using ScreenFactory = std::function<std::unique_ptr<Screen>(ScreenRequest)>;

    explicit ScreenManager(ScreenFactory factory);

    void requestScreen(ScreenRequest request);

    void update(float dt, const input::InputSnapshot& input);
    void draw(sf::RenderWindow &window);

    [[nodiscard]] bool hasScreen() const;
    [[nodiscard]] Screen *getCurrentScreen() const;
    [[nodiscard]] bool shouldQuit() const;

   private:
    void handleRequest(ScreenRequest request);

    ScreenFactory m_factory;
    std::unique_ptr<Screen> m_currentScreen;
    bool m_shouldQuit = false;
    Utils::Logging::Logger m_logger{"ScreenManager"};
};

}  // namespace pacman::client::screen
