#pragma once

#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "client/screen/Screen.hpp"

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

#include <Utils/Logging/LoggerSubscribed.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

namespace pacman::client::screen {
class ScreenManager;
}

namespace pacman::client::screens {

// Shown between LobbyScreen and GameScreen while the client loads assets,
// parses the map, initialises the local ECS, and waits for the server to
// confirm all players are ready.
//
// Lifecycle:
//   onEnter → RAII subscription to ClientNetwork events begins at construction
//   update  → advances through loading steps; sends ReadyToPlay when done
//   onExit  → subscription ends at destruction
//
// Transitions:
//   GameSnapshotEvent received → all players ready → setScreen<GameScreen>
//   ServerShutdownEvent / DisconnectedEvent → setScreen<MenuScreen>
class LoadingScreen
    : public screen::Screen,
      public Utils::PublishSubscribe::ISubscriber<network::events::ClientNetworkEvent> {
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

  // ISubscriber<ClientNetworkEvent>
  void onUpdate(const network::events::ClientNetworkEvent &event) override;

private:
  [[maybe_unused]] screen::ScreenManager &m_screenManager;
  network::ClientNetwork &m_network;
  core::protocol::GameStartPacket m_gameStart;
  [[maybe_unused]] core::PlayerId m_localPlayerId;

  bool m_mapParsed = false;
  bool m_assetsLoaded = false;
  bool m_simInitialized = false;
  bool m_readyToPlaySent = false;

  Utils::Logging::LoggerSubscribed m_logger{"LoadingScreen"};
};

} // namespace pacman::client::screens
