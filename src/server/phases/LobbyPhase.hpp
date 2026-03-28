#pragma once

#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"

#include "core/maps/Map.hpp"

#include <Utils/Logging/LoggerSubscribed.h>

#include <array>
#include <memory>
#include <optional>
#include <string>

namespace pacman::server::app {
class ServerApp;
}

namespace pacman::server::phases {

class LobbyPhase : public Phase, public network::INetworkEventHandler {
public:
  LobbyPhase(app::ServerApp &app, network::ServerNetwork &network);

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

  std::optional<core::maps::Map> m_map; // loaded in onEnter()

  struct PlayerSlot {
    core::PlayerId id = 0;
    std::string name;
    bool connected = false;
    bool ready = false;
  };
  std::array<PlayerSlot, core::maxPlayers> m_slots{};
  uint8_t m_playerCount = 0;

  Utils::Logging::LoggerSubscribed m_logger{"LobbyPhase"};
};

} // namespace pacman::server::phases
