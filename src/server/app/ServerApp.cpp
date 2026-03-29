#include "server/app/ServerApp.hpp"
#include "server/phases/LobbyPhase.hpp"

#include <Utils/Config/ConfigManagerWithLogger.h>
#include <Utils/Config/Providers/CLIConfigProvider.h>
#include <Utils/Config/Providers/JsonConfigProvider.h>
#include <Utils/Logging/LoggerMacros.h>
#include <Utils/Providers/FileSourceProvider.h>

#include <chrono>
#include <csignal>
#include <thread>

namespace {

pacman::server::app::ServerApp *g_app = nullptr;

void signalHandler(int /*sig*/) {
  if (g_app)
    g_app->stop();
}

} // namespace

namespace pacman::server::app {

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
  using JsonProvider =
      Utils::Config::Providers::JsonConfigProvider<ServerConfig>;
  using Manager =
      Utils::Config::ConfigManagerWithLogger<ServerConfig, CLIProvider,
                                             JsonProvider>;

  // FileSourceProvider starts with the default config path.
  // After CLI is parsed, setPath() updates it if --config was passed.
  auto fileSource = std::make_unique<Utils::Providers::FileSourceProvider>(
      "config/server.json");
  auto *fileSourcePtr = fileSource.get();

  auto manager = std::make_unique<Manager>(
      std::make_unique<CLIProvider>(),
      std::make_unique<JsonProvider>(std::move(fileSource)));

  manager->update<CLIProvider>(argc, argv);

  // Propagate CLI-resolved config path to the file source provider.
  fileSourcePtr->setPath(m_config->configPath.get());

  // First run() loads the JSON file.
  manager->run();

  m_configManager = std::move(manager);

  setPhase(std::make_unique<phases::LobbyPhase>(*this, m_network));
}

void ServerApp::run() {
  LOG_I("ServerApp starting main loop at {}Hz", m_config->tickRate.get());
  if (!m_network.start(static_cast<uint16_t>(m_config->port.get()),
                       m_config->maxPlayers.get())) {
    LOG_E("Failed to start network on port {}", m_config->port.get());
    return;
  }
  m_running = true;

  const float tickDt = 1.0f / m_config->tickRate.get();
  const auto tickDuration = std::chrono::duration<float>(tickDt);

  using Clock = std::chrono::steady_clock;
  auto previousTime = Clock::now();
  float accumulator = 0.0f;

  while (m_running) {
    const auto currentTime = Clock::now();
    float elapsed =
        std::chrono::duration<float>(currentTime - previousTime).count();
    previousTime = currentTime;
    if (elapsed > 0.1f)
      elapsed = 0.1f;
    accumulator += elapsed;

    if (m_pendingPhase) {
      if (m_currentPhase)
        m_currentPhase->onExit();
      m_currentPhase = std::move(m_pendingPhase);
      m_currentPhase->onEnter();
      LOG_I("Phase transition complete");
    }

    m_configManager->run();
    m_network.poll();

    while (accumulator >= tickDt) {
      if (m_currentPhase)
        m_currentPhase->update(tickDt);
      ++m_tick;
      accumulator -= tickDt;
    }

    const auto frameEnd = Clock::now();
    if (const auto frameDuration = frameEnd - currentTime;
        frameDuration < tickDuration)
      std::this_thread::sleep_for(tickDuration - frameDuration);

    if (m_tick % 600 == 0 && m_tick > 0)
      LOG_D("Server tick: {}", m_tick);
  }

  if (m_currentPhase) {
    m_currentPhase->onExit();
    m_currentPhase.reset();
  }

  LOG_I("ServerApp main loop ended (tick={})", m_tick);
}

// ---------------------------------------------------------------------------
// Phase management & lifecycle
// ---------------------------------------------------------------------------

void ServerApp::stop() {
  LOG_I("ServerApp stopping");
  m_running = false;
}

void ServerApp::setPhase(std::unique_ptr<phases::Phase> phase) {
  m_pendingPhase = std::move(phase);
}

} // namespace pacman::server::app
