#pragma once

#include "client/network/ClientNetwork.hpp"
#include "client/screen/Screen.hpp"

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <memory>

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

// Shows the pre-game lobby: player list, ready toggle, Start button (host
// only).
//
// Lifecycle:
//   onEnter → registers as ClientNetwork listener
//   onExit  → deregisters listener
//
// Transitions:
//   onGameStart received → setScreen<LoadingScreen>
//   onServerShutdown / onDisconnected → setScreen<MenuScreen>
class LobbyScreen : public screen::Screen,
                    public network::IClientNetworkListener {
public:
  LobbyScreen(
      screen::ScreenManager &screenManager, network::ClientNetwork &network,
      core::PlayerId localPlayerId, bool isHost,
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);

  // Screen
  void onEnter() override;
  void onExit() override;
  void handleEvent(const sf::Event &event) override;
  void update(float dt) override;
  void draw(sf::RenderWindow &window) override;

  // IClientNetworkListener
  void onConnected(core::PlayerId assignedId) override;
  void onDisconnected() override;
  void onLobbyState(const core::protocol::LobbyStatePacket &packet) override;
  void onGameStart(const core::protocol::GameStartPacket &packet) override;
  void
  onGameSnapshot(const core::protocol::GameSnapshotPacket &packet) override;
  void onRoundEnd(const core::protocol::RoundEndPacket &packet) override;
  void
  onServerShutdown(const core::protocol::ServerShutdownPacket &packet) override;

private:
  screen::ScreenManager &m_screenManager;
  network::ClientNetwork &m_network;
  core::PlayerId m_localPlayerId;
  bool m_isHost;
  std::shared_ptr<Utils::Logging::LoggerConfig> m_loggerConfig;

  core::protocol::LobbyStatePacket m_lobbyState{};
  bool m_localReady = false;

  Utils::Logging::Logger m_logger;
};

} // namespace pacman::client::screens
