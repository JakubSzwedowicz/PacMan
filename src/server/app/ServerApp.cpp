#include "server/app/ServerApp.hpp"
#include "server/phases/LobbyPhase.hpp"

#include <Utils/Config/ConfigManagerWithLogger.h>
#include <Utils/Config/ConfigPublisher.h>
#include <Utils/Config/Providers/CLIConfigProvider.h>
#include <Utils/Config/Providers/JsonConfigProvider.h>
#include <Utils/Logging/LoggerMacros.h>

#include <chrono>
#include <csignal>
#include <fstream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

pacman::server::app::ServerApp *g_app = nullptr;

void signalHandler(int /*sig*/) {
  if (g_app) g_app->stop();
}

std::string findConfigPath(int argc, char *argv[]) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::string_view(argv[i]) == "--config") return argv[i + 1];
  }
  return "config/server.json";
}

// Translate --map <name> → --map-path assets/maps/<name>.json.
std::vector<std::string> preprocessArgs(int argc, char *argv[]) {
  std::vector<std::string> out;
  for (int i = 1; i < argc; ++i) {
    std::string_view arg(argv[i]);
    if (arg == "--map" && i + 1 < argc) {
      out.push_back("--map-path");
      out.push_back(std::string("assets/maps/") + argv[++i] + ".json");
    } else {
      out.push_back(std::string(arg));
    }
  }
  return out;
}

} // namespace

namespace pacman::server::app {

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int ServerApp::main(int argc, char *argv[]) {
  g_app = this;
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  init(argc, argv);
  run();

  g_app = nullptr;
  return 0;
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void ServerApp::init(int argc, char *argv[]) {
  using CLIProvider =
      Utils::Config::Providers::CLIConfigProvider<core::ServerConfig>;
  using JsonProvider =
      Utils::Config::Providers::JsonConfigProvider<core::ServerConfig>;

  Utils::Config::ConfigManagerWithLogger<core::ServerConfig, CLIProvider,
                                         JsonProvider>
      manager;

  std::string configPath = findConfigPath(argc, argv);
  std::ifstream file(configPath);
  if (file.is_open()) {
    manager.update<JsonProvider>(file);
  }

  auto modifiedArgs = preprocessArgs(argc, argv);
  std::vector<char *> argvPtrs;
  argvPtrs.push_back(argv[0]);
  for (auto &s : modifiedArgs) argvPtrs.push_back(s.data());
  manager.update<CLIProvider>(static_cast<int>(argvPtrs.size()),
                               argvPtrs.data());

  m_config =
      static_cast<Utils::Config::ConfigPublisher<core::ServerConfig> &>(manager)
          .getConfig();

  if (!m_config->renderAscii.get()) {
    m_config->renderInterval.set(500);
  }

  setPhase(std::make_unique<phases::LobbyPhase>(*this, m_network));

  // Re-publish logger config to all LoggerSubscribed instances created so far.
  manager.resolve();
}

// ---------------------------------------------------------------------------
// Game loop
// ---------------------------------------------------------------------------

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
    if (elapsed > 0.1f) elapsed = 0.1f;
    accumulator += elapsed;

    if (m_pendingPhase) {
      if (m_currentPhase) m_currentPhase->onExit();
      m_currentPhase = std::move(m_pendingPhase);
      m_currentPhase->onEnter();
      LOG_I("Phase transition complete");
    }

    m_network.poll();

    while (accumulator >= tickDt) {
      if (m_currentPhase) m_currentPhase->update(tickDt);
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
