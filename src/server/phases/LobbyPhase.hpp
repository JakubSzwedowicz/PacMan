#pragma once

#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"

#include "core/Config.hpp"
#include "core/protocol/Packets.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <array>
#include <memory>
#include <string>

namespace pacman::server::app {
class ServerApp;
}

namespace pacman::server::phases {

class LobbyPhase : public Phase,
                   public network::INetworkEventHandler {
public:
  LobbyPhase(
      app::ServerApp &app, network::ServerNetwork &network,
      const core::ServerConfig &config,
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  // Phase
  void onEnter() override;
  void onExit() override;
  void update(float dt) override;

  // INetworkEventHandler — only lobby-relevant events
  void onPlayerConnect(core::PlayerId id) override;
  void onPlayerDisconnect(core::PlayerId id) override;
  void onLobbyReady(core::PlayerId id, bool ready) override;

private:
  void broadcastLobbyState();
  [[nodiscard]] bool allPlayersReady() const;
  void startGame();

  app::ServerApp &m_app;
  network::ServerNetwork &m_network;
  const core::ServerConfig &m_config;

  struct PlayerSlot {
    core::PlayerId id = 0;
    std::string name;
    bool connected = false;
    bool ready = false;
  };
  std::array<PlayerSlot, core::maxPlayers> m_slots{};
  uint8_t m_playerCount = 0;

  Utils::Logging::Logger m_logger;
};

} // namespace pacman::server::phases
