#include "server/app/ServerApp.hpp"

#include <Utils/Config/ConfigManager.h>
#include <Utils/Config/ConfigProviders/CLIConfigProvider.h>
#include <Utils/Config/ConfigProviders/JsonConfigProvider.h>
#include <Utils/Logging/LoggerMacros.h>
#include <Utils/Providers/FileSourceProvider.h>
#include <signal.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace {

// Only the atomic flag is touched from the signal handler — no logging,
// no function calls that might hold a mutex.
std::atomic<bool> *g_running = nullptr;

void signalHandler(int /*sig*/) {
    if (g_running) g_running->store(false, std::memory_order_relaxed);
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
    g_running = &m_running;
    // Use sigaction without SA_RESTART so that blocking syscalls (nanosleep,
    // ENet's select/poll) return EINTR immediately when the signal fires,
    // letting the main loop see m_running == false without waiting for the
    // current sleep/poll to time out on its own.
    struct sigaction sa {};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);

    init(argc, argv);
    LOG_I("PacMan Server initialized");
    run();

    g_running = nullptr;
    return 0;
}

void ServerApp::init(int argc, char *argv[]) {
    using CLIProvider = Utils::Config::ConfigProviders::CLIConfigProvider<ServerConfig>;
    using JsonProvider = Utils::Config::ConfigProviders::JsonConfigProvider<ServerConfig>;
    using Manager = Utils::Config::ConfigManager<ServerConfig, CLIProvider, JsonProvider>;

    // Extract configPath from CLI args (default: config/server.json).
    std::string configPath = "config/server.json";
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--configPath") {
            configPath = argv[i + 1];
            break;
        }
    }

    // Create FileSourceProvider with the correct path BEFORE ConfigManager
    // so loggers created during resolve use the correct config from JSON.
    auto fileSource = std::make_unique<Utils::Providers::FileSourceProvider>(configPath);

    auto manager = std::make_unique<Manager>(std::make_unique<CLIProvider>(argc, argv),
                                             std::make_unique<JsonProvider>(std::move(fileSource)));
    manager->run();

    m_configManager = std::move(manager);

    LOG_I("ServerApp initializing");

    // Freeze game settings from the resolved config. Config hot-reload will not
    // affect the running game session — only the server loop timing (tickDt).
    game::GameSettings settings{
        m_config->mapPath.get(),
        m_config->maxPlayers.get(),
        m_config->renderAscii.get(),
        m_config->renderInterval.get(),
    };
    m_gameRunner = std::make_unique<game::GameRunner>(std::move(settings), m_network);
    LOG_I("ServerApp initialized");
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
