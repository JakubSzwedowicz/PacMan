#pragma once

#include "server/ai/AISystem.hpp"
#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"

#include "core/Config.hpp"
#include "core/maps/Map.hpp"
#include "core/protocol/Packets.hpp"
#include "core/simulation/Simulation.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <entt/entt.hpp>

#include <array>
#include <memory>
#include <unordered_map>

namespace pacman::server::app {
class ServerApp;
}

namespace pacman::server::phases {

class GamePhase : public Phase,
                  public network::INetworkEventHandler {
public:
  GamePhase(
      app::ServerApp &app, network::ServerNetwork &network, core::maps::Map map,
      std::array<core::protocol::PlayerInfo, core::maxPlayers> players,
      uint8_t playerCount,
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  // Phase
  void onEnter() override;
  void onExit() override;
  void update(float dt) override;

  // INetworkEventHandler — only game-relevant events
  void onPlayerDisconnect(core::PlayerId id) override;
  void onPlayerInput(const core::protocol::PlayerInputPacket &packet) override;

private:
  void spawnEntities();
  void applyPendingInputs();
  void broadcastSnapshot();
  [[nodiscard]] bool isRoundOver() const;
  void endRound();
  [[nodiscard]] core::protocol::GameSnapshotPacket buildSnapshot() const;
  [[nodiscard]] core::protocol::RoundEndPacket buildRoundEnd() const;

  app::ServerApp &m_app;
  network::ServerNetwork &m_network;
  core::maps::Map m_map;

  entt::registry m_registry;
  core::simulation::Simulation m_simulation;
  ai::AISystem m_aiSystem;

  std::array<core::protocol::PlayerInfo, core::maxPlayers> m_players{};
  uint8_t m_playerCount = 0;
  std::unordered_map<core::PlayerId, entt::entity> m_playerEntities;
  std::unordered_map<core::PlayerId, core::protocol::PlayerInputPacket>
      m_pendingInputs;

  core::Tick m_tick = 0;
  float m_snapshotAccumulator = 0.0f;
  static constexpr float snapshotRate = 1.0f / 20.0f;

  Utils::Logging::Logger m_logger;
};

} // namespace pacman::server::phases
