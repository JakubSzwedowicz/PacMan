#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>
#include <Utils/Runnables/IRunnable.h>

#include <cstdint>

#include "core/network/ENetSourceProvider.hpp"
#include "core/protocol/Packets.hpp"
#include "server/network/NetworkEventParser.hpp"
#include "server/network/NetworkEvents.hpp"

namespace pacman::server::network {

// Bridges ENet ↔ pub/sub.
//
// Receive path (IRunnable::run(), once per tick):
//   ENetSourceProvider::run() drains ENet events into its internal queue.
//   NetworkEventParser converts each RawNetworkMessage into a ServerNetworkEvent.
//   ServerNetwork publishes each event to ISubscriber<ServerNetworkEvent>.
//
// Send path: sendTo / broadcast delegate directly to m_enetSource.
//
// Active phases subscribe via ISubscriber<ServerNetworkEvent> RAII.
class ServerNetwork : public Utils::PublishSubscribe::IPublisher<events::ServerNetworkEvent>,
                      public Utils::Runnables::IRunnable {
   public:
    ServerNetwork() = default;
    ~ServerNetwork();

    ServerNetwork(const ServerNetwork &) = delete;
    ServerNetwork &operator=(const ServerNetwork &) = delete;
    ServerNetwork(ServerNetwork &&) = delete;
    ServerNetwork &operator=(ServerNetwork &&) = delete;

    [[nodiscard]] bool start(uint16_t port, int maxClients);
    void stop();
    [[nodiscard]] bool isRunning() const;

    // Outgoing (implementations added in Phase 4)
    void sendLobbyState(const core::protocol::LobbyStatePacket &packet);
    void broadcastGameStart(core::protocol::GameStartPacket pkt);
    void broadcastSnapshot(const core::protocol::GameSnapshotPacket &packet);
    void broadcastRoundEnd(const core::protocol::RoundEndPacket &packet);
    void broadcastShutdown(const core::protocol::ServerShutdownPacket &packet);

    // IRunnable — drains ENet, parses events, publishes to subscribers.
    // Called once per server tick from ServerApp.
    void run() override;

   private:
    core::network::ENetSourceProvider m_enetSource;
    NetworkEventParser m_parser;

    Utils::Logging::Logger m_logger{"ServerNetwork"};
};

}  // namespace pacman::server::network
