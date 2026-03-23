#include "server/app/ServerApp.hpp"
#include "core/Common.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <chrono>
#include <thread>

namespace pacman::server::app {

ServerApp::ServerApp(core::ServerConfig config,
                     std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig)
    : m_config(std::move(config)), m_loggerConfig(std::move(loggerConfig)),
      m_network(m_loggerConfig), m_logger("ServerApp", m_loggerConfig) {
  LOG_I("ServerApp created (port={}, maxPlayers={})", m_config.port,
        m_config.maxPlayers);
}

void ServerApp::run() {
  LOG_I("ServerApp starting main loop at {}Hz", m_config.tickRate);
  if (!m_network.start(static_cast<uint16_t>(m_config.port), m_config.maxPlayers)) {
    LOG_E("Failed to start network on port {}", m_config.port);
    return;
  }
  m_running = true;

  float tickDt = 1.0f / m_config.tickRate;
  auto tickDuration = std::chrono::duration<float>(tickDt);

  using Clock = std::chrono::steady_clock;
  auto previousTime = Clock::now();
  float accumulator = 0.0f;

  while (m_running) {
    auto currentTime = Clock::now();
    float elapsed =
        std::chrono::duration<float>(currentTime - previousTime).count();
    previousTime = currentTime;

    if (elapsed > 0.1f) {
      elapsed = 0.1f;
    }
    accumulator += elapsed;

    if (m_pendingPhase) {
      if (m_currentPhase) {
        m_currentPhase->onExit();
      }
      m_currentPhase = std::move(m_pendingPhase);
      m_currentPhase->onEnter();
      LOG_I("Phase transition complete");
    }

    m_network.poll();

    while (accumulator >= tickDt) {
      if (m_currentPhase) {
        m_currentPhase->update(tickDt);
      }
      m_tick++;
      accumulator -= tickDt;
    }

    auto frameEnd = Clock::now();
    auto frameDuration = frameEnd - currentTime;
    if (frameDuration < tickDuration) {
      std::this_thread::sleep_for(tickDuration - frameDuration);
    }

    if (m_tick % 600 == 0 && m_tick > 0) {
      LOG_D("Server tick: {}", m_tick);
    }
  }

  if (m_currentPhase) {
    m_currentPhase->onExit();
    m_currentPhase.reset();
  }

  LOG_I("ServerApp main loop ended (tick={})", m_tick);
}

void ServerApp::stop() {
  LOG_I("ServerApp stopping");
  m_running = false;
}

void ServerApp::setPhase(std::unique_ptr<phases::Phase> phase) {
  m_pendingPhase = std::move(phase);
}

} // namespace pacman::server::app
