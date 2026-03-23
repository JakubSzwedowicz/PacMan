#pragma once

#include "core/Common.hpp"
#include "core/Config.hpp"
#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <atomic>
#include <memory>

namespace pacman::server::app {

class ServerApp {
public:
  ServerApp(core::ServerConfig config,
            std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig);

  void run();
  void stop();

  void setPhase(std::unique_ptr<phases::Phase> phase);

  // Dependency injection point for phases — they hold a reference to this.
  [[nodiscard]] network::ServerNetwork &network() { return m_network; }
  [[nodiscard]] const core::ServerConfig &config() const { return m_config; }

private:
  core::ServerConfig m_config;
  std::shared_ptr<Utils::Logging::LoggerConfig> m_loggerConfig;

  network::ServerNetwork m_network;

  std::unique_ptr<phases::Phase> m_currentPhase;
  std::unique_ptr<phases::Phase> m_pendingPhase;

  core::Tick m_tick = 0;
  std::atomic<bool> m_running{false};

  Utils::Logging::Logger m_logger;
};

} // namespace pacman::server::app
