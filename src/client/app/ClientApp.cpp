#include "client/app/ClientApp.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <SFML/System/Clock.hpp>
#include <stdexcept>

#include "client/input/InputManager.hpp"
#include "core/ConfigUtils.hpp"
#include "core/maps/MapsManager.hpp"

namespace pacman::client {

int ClientApp::main(int argc, char* argv[]) {
    init(argc, argv);
    run();
    return 0;
}

void ClientApp::init(int argc, char* argv[]) {
    m_configManager = core::initializeConfigManager<ClientConfig>(argc, argv, "config/client.json");

    if (!m_config) {
        throw std::runtime_error("Client configuration was not resolved");
    }

    unsigned int windowWidth = static_cast<unsigned int>(m_config->windowWidth.get());
    unsigned int windowHeight = static_cast<unsigned int>(m_config->windowHeight.get());

    if (auto mapResult = core::maps::MapsManager::loadFromFile(m_config->mapPath.get()); mapResult) {
        windowWidth = static_cast<unsigned int>(mapResult->width * mapResult->tileSize);
        windowHeight = static_cast<unsigned int>(mapResult->height * mapResult->tileSize);
    } else {
        LOG_W("Falling back to configured window size because map '{}' could not be loaded: {}", m_config->mapPath.get(),
              mapResult.error());
    }

    m_appWindow.emplace("PacMan", windowWidth, windowHeight);

    m_screenManager.requestScreen(screen::OpenMenuRequest{});

    LOG_I("ClientApp initialised");
}

void ClientApp::run() {
    LOG_I("ClientApp starting main loop");

    input::InputMapper inputMapper;
    core::Tick tick = 0;
    sf::Clock deltaClock;
    float accumulator = 0.0f;

    while (m_appWindow->isOpen() && !m_screenManager.shouldQuit()) {
        sf::Time frameTime = deltaClock.restart();
        float dt = frameTime.asSeconds();
        if (dt > 0.1f) dt = 0.1f;
        accumulator += dt;

        m_configManager->run();
        m_network.run();

        m_appWindow->pollEvents([&inputMapper](const sf::Event& event) { inputMapper.feed(event); });

        while (accumulator >= core::tickDt) {
            auto input = inputMapper.snapshot(tick);
            m_screenManager.update(core::tickDt, input);
            ++tick;
            accumulator -= core::tickDt;
        }

        m_appWindow->renderFrame(frameTime, [this](sf::RenderWindow& rw) { m_screenManager.draw(rw); });
    }

    m_appWindow->shutdown();
    LOG_I("ClientApp main loop ended (tick={})", tick);
}

}  // namespace pacman::client
