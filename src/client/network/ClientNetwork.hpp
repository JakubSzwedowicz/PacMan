#pragma once

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

#include <Utils/Logging/LoggerSubscribed.h>

#include <cstdint>
#include <string>

namespace pacman::client::network {

// ---------------------------------------------------------------------------
// IClientNetworkListener
// Implement this on any screen that needs to react to server messages.
// Register via ClientNetwork::setListener() in Screen::onEnter(),
// and deregister (pass nullptr) in Screen::onExit().
// ---------------------------------------------------------------------------
class IClientNetworkListener {
public:
  virtual ~IClientNetworkListener() = default;

  virtual void onConnected(core::PlayerId assignedId) = 0;
  virtual void onDisconnected() = 0;

  virtual void onLobbyState(const core::protocol::LobbyStatePacket &packet) = 0;
  virtual void onGameStart(const core::protocol::GameStartPacket &packet) = 0;
  virtual void
  onGameSnapshot(const core::protocol::GameSnapshotPacket &packet) = 0;
  virtual void onRoundEnd(const core::protocol::RoundEndPacket &packet) = 0;
  virtual void
  onServerShutdown(const core::protocol::ServerShutdownPacket &packet) = 0;
};

// ---------------------------------------------------------------------------
// ClientNetwork
// Wraps an ENet peer. Owned by ClientApp; screens receive a reference.
// Call poll() once per frame (inside the ClientApp event loop) to pump
// incoming ENet events and invoke the registered listener.
// ---------------------------------------------------------------------------
class ClientNetwork {
public:
  ClientNetwork();
  ~ClientNetwork();

  ClientNetwork(const ClientNetwork &) = delete;
  ClientNetwork &operator=(const ClientNetwork &) = delete;
  ClientNetwork(ClientNetwork &&) = delete;
  ClientNetwork &operator=(ClientNetwork &&) = delete;

  // Blocks until connection succeeds or times out (~5 s).
  [[nodiscard]] bool connect(const std::string &host, uint16_t port);
  void disconnect();
  [[nodiscard]] bool isConnected() const;

  // Register the active screen as the event sink.
  // Only one listener is active at a time; pass nullptr to deregister.
  void setListener(IClientNetworkListener *listener);

  // Outgoing messages
  void sendLobbyReady(bool ready);
  void sendReadyToPlay();
  void sendInput(const core::protocol::PlayerInputPacket &packet);

  // Must be called once per frame to pump ENet events.
  void poll();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl; // hides ENet headers from consumers
  IClientNetworkListener *m_listener = nullptr;
  Utils::Logging::LoggerSubscribed m_logger{"ClientNetwork"};
};

} // namespace pacman::client::network
