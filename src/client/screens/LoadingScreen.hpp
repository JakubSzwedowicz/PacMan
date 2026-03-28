#pragma once

#include "client/network/ClientNetwork.hpp"
#include "client/screen/Screen.hpp"

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

#include <Utils/Logging/LoggerSubscribed.h>


namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

// Shown between LobbyScreen and GameScreen while the client loads assets,
// parses the map, initialises the local ECS, and waits for the server to
// confirm all players are ready.
//
// Lifecycle:
//   onEnter → registers as ClientNetwork listener, starts loading
//   update  → advances through loading steps; sends ReadyToPlay when done
//   onExit  → deregisters listener
//
// Transitions:
//   onGameSnapshot received → all players ready → setScreen<GameScreen>
//   onServerShutdown / onDisconnected → setScreen<MenuScreen>
class LoadingScreen : public screen::Screen,
                      public network::IClientNetworkListener {
public:
  LoadingScreen(screen::ScreenManager &screenManager,
                network::ClientNetwork &network,
                core::protocol::GameStartPacket gameStart,
                core::PlayerId localPlayerId);

  // Screen
  void onEnter() override;
  void onExit() override;
  void handleEvent(const sf::Event &event) override;
  void update(float dt) override;
  void draw(sf::RenderWindow &window) override;

  // IClientNetworkListener — only the relevant callbacks are non-trivial
  void onConnected(core::PlayerId) override {}
  void onDisconnected() override;
  void onLobbyState(const core::protocol::LobbyStatePacket &) override {}
  void onGameStart(const core::protocol::GameStartPacket &) override {}
  void
  onGameSnapshot(const core::protocol::GameSnapshotPacket &packet) override;
  void onRoundEnd(const core::protocol::RoundEndPacket &) override {}
  void
  onServerShutdown(const core::protocol::ServerShutdownPacket &packet) override;

private:
  screen::ScreenManager &m_screenManager;
  network::ClientNetwork &m_network;
  core::protocol::GameStartPacket m_gameStart;
  core::PlayerId m_localPlayerId;

  // Loading steps, ticked off in order during update()
  bool m_mapParsed = false;
  bool m_assetsLoaded = false;
  bool m_simInitialized = false;
  bool m_readyToPlaySent = false;

  Utils::Logging::LoggerSubscribed m_logger{"LoadingScreen"};
};

} // namespace pacman::client::screens
