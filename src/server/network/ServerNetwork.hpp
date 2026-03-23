#pragma once

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

#include <Utils/Logging/Logger.h>
#include <Utils/Logging/LoggerConfig.h>

#include <cstdint>
#include <memory>

namespace pacman::server::network {

// ---------------------------------------------------------------------------
// INetworkEventHandler
// Implemented by whichever Phase is currently active. All methods have
// default empty bodies — phases only override what they care about.
// ---------------------------------------------------------------------------
class INetworkEventHandler {
public:
  virtual ~INetworkEventHandler() = default;

  virtual void onPlayerConnect(core::PlayerId /*id*/) {}
  virtual void onPlayerDisconnect(core::PlayerId /*id*/) {}
  virtual void onLobbyReady(core::PlayerId /*id*/, bool /*ready*/) {}
  virtual void onReadyToPlay(core::PlayerId /*id*/) {}
  virtual void onPlayerInput(const core::protocol::PlayerInputPacket &) {}
};

// ---------------------------------------------------------------------------
// ServerNetwork
// Wraps an ENet host. Owned by ServerApp; phases receive a reference.
// Call poll() once per server tick to pump incoming ENet events.
// Uses the Null Object pattern internally — m_handler is never null.
// ---------------------------------------------------------------------------
class ServerNetwork {
public:
  explicit ServerNetwork(
      std::shared_ptr<Utils::Logging::LoggerConfig> loggerConfig = nullptr);
  ~ServerNetwork();

  ServerNetwork(const ServerNetwork &) = delete;
  ServerNetwork &operator=(const ServerNetwork &) = delete;
  ServerNetwork(ServerNetwork &&) = delete;
  ServerNetwork &operator=(ServerNetwork &&) = delete;

  [[nodiscard]] bool start(uint16_t port, int maxClients);
  void stop();
  [[nodiscard]] bool isRunning() const;

  // Register the active phase as the event handler.
  void setHandler(INetworkEventHandler &handler);
  // Reset to the internal no-op Null Object.
  void clearHandler();

  // Outgoing messages
  void sendLobbyState(const core::protocol::LobbyStatePacket &packet);
  void broadcastGameStart(
      const core::protocol::GameStartPacket &templatePkt,
      const std::array<core::PlayerId, core::maxPlayers> &playerIds,
      uint8_t count);
  void broadcastSnapshot(const core::protocol::GameSnapshotPacket &packet);
  void broadcastRoundEnd(const core::protocol::RoundEndPacket &packet);
  void broadcastShutdown(const core::protocol::ServerShutdownPacket &packet);

  // Must be called once per server tick.
  void poll();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl; // hides ENet headers from consumers
  INetworkEventHandler *m_handler = nullptr; // always points to valid handler
  Utils::Logging::Logger m_logger;
};

} // namespace pacman::server::network
