#include "server/app/ServerApp.hpp"

#include <Utils/Config/ConfigManagerWithLogger.h>
#include <Utils/Config/Providers/CLIConfigProvider.h>
#include <Utils/Config/Providers/JsonConfigProvider.h>
#include <Utils/Logging/LoggerMacros.h>
#include <Utils/Providers/FileSourceProvider.h>

#include <algorithm>
#include <chrono>
#include <csignal>
#include <thread>

namespace {

pacman::server::app::ServerApp *g_app = nullptr;

void signalHandler(int /*sig*/) {
    if (g_app) g_app->stop();
}

}  // namespace

namespace pacman::server::app {

void ServerApp::onUpdate(
    const std::shared_ptr<const ServerConfig> &config) {  // It doesn't set any mutex yet but it will be once the app
                                                          // becomes multithreaded. If ever...
    m_config = config;

    const float dt = 1.0f / config->tickRate.get();
    m_loopSettings = LoopSettings{
        dt,
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<float>(dt)),
    };
}

int ServerApp::main(int argc, char *argv[]) {
    g_app = this;
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    init(argc, argv);
    run();

    g_app = nullptr;
    return 0;
}

void ServerApp::init(int argc, char *argv[]) {
    using CLIProvider = Utils::Config::Providers::CLIConfigProvider<ServerConfig>;
    using JsonProvider = Utils::Config::Providers::JsonConfigProvider<ServerConfig>;
    using Manager = Utils::Config::ConfigManagerWithLogger<ServerConfig, CLIProvider, JsonProvider>;

    auto fileSource = std::make_unique<Utils::Providers::FileSourceProvider>("");
    auto *fileSourcePtr = fileSource.get();

    auto manager = std::make_unique<Manager>(std::make_unique<CLIProvider>(),
                                             std::make_unique<JsonProvider>(std::move(fileSource)));

    manager->update<CLIProvider>(argc, argv);
    fileSourcePtr->setPath(m_config->configPath.get());
    manager->run();

    m_configManager = std::move(manager);

    // Freeze game settings from the resolved config. Config hot-reload will not
    // affect the running game session — only the server loop timing (tickDt).
    game::GameSettings settings{
        m_config->mapPath.get(),
        m_config->maxPlayers.get(),
        m_config->renderAscii.get(),
        m_config->renderInterval.get(),
    };
    m_gameRunner = std::make_unique<game::GameRunner>(std::move(settings), m_network);
}

void ServerApp::run() {
    if (!m_network.start(static_cast<uint16_t>(m_config->port.get()), m_config->maxPlayers.get())) {
        LOG_E("Failed to start network on port {}", m_config->port.get());
        return;
    }
    m_running = true;
    LOG_I("Server loop starting at {}Hz", m_config->tickRate.get());

    auto lastTime = Clock::now();
    float accumulator = 0.0f;

    while (m_running) {
        const auto frameStart = Clock::now();
        accumulator += advanceClock(lastTime, frameStart);

        pumpConfigAndNetwork();
        stepSimulation(accumulator);
        sleepRemainder(frameStart);
    }

    m_gameRunner->shutdown();
    LOG_I("Server loop ended (tick={})", m_tick);
}

float ServerApp::advanceClock(Clock::time_point &last, Clock::time_point now) const {
    const float elapsed = std::chrono::duration<float>(now - last).count();
    last = now;
    return std::min(elapsed, 0.1f);
}

void ServerApp::pumpConfigAndNetwork() {
    m_configManager->run();
    m_network.run();
}

void ServerApp::stepSimulation(float &accumulator) {
    const float dt = m_loopSettings.tickDt;
    while (accumulator >= dt) {
        m_gameRunner->update(dt);
        ++m_tick;
        accumulator -= dt;
    }
    if (m_tick % 600 == 0 && m_tick > 0) LOG_D("Server tick: {}", m_tick);
}

void ServerApp::sleepRemainder(Clock::time_point frameStart) const {
    const auto elapsed = Clock::now() - frameStart;
    if (elapsed < m_loopSettings.tickDuration) std::this_thread::sleep_for(m_loopSettings.tickDuration - elapsed);
}

void ServerApp::stop() {
    LOG_I("ServerApp stopping");
    m_running = false;
}

}  // namespace pacman::server::app
