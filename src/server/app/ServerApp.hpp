#pragma once

#include "core/Common.hpp"
#include "core/Config.hpp"
#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"

#include <Utils/Logging/LoggerSubscribed.h>

#include <atomic>
#include <memory>

namespace pacman::server::app {

class ServerApp {
public:
  // Full server lifecycle: signal setup → init → run.
  // Call from int main() — never call run() or init() directly.
  int main(int argc, char *argv[]);

  // Called by the OS signal handler to request a clean shutdown.
  void stop();

  // Called by phases to schedule a phase transition on the next tick.
  void setPhase(std::unique_ptr<phases::Phase> phase);

  [[nodiscard]] network::ServerNetwork &network() { return m_network; }
  [[nodiscard]] const core::ServerConfig &config() const { return *m_config; }

private:
  // Parses args, resolves config, starts network, creates initial phase.
  void init(int argc, char *argv[]);

  // Fixed-timestep game loop — blocks until stop() is called.
  void run();

  std::shared_ptr<core::ServerConfig> m_config; // set in init()
  network::ServerNetwork m_network;

  std::unique_ptr<phases::Phase> m_currentPhase;
  std::unique_ptr<phases::Phase> m_pendingPhase;

  core::Tick m_tick = 0;
  std::atomic<bool> m_running{false};

  Utils::Logging::LoggerSubscribed m_logger{"ServerApp"};
};

} // namespace pacman::server::app
