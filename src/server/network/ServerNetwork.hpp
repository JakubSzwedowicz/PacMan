#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/Providers/QueuedResourceProvider.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>
#include <Utils/Runnables/IRunnable.h>

#include <array>
#include <cstdint>
#include <memory>

#include "core/Common.hpp"
#include "core/network/ENetSourceProvider.hpp"
#include "core/network/RawNetworkMessage.hpp"
#include "core/protocol/Packets.hpp"
#include "server/network/NetworkEvents.hpp"

namespace pacman::server::network {

// Bridges ENet ↔ pub/sub.
//
// Receive path (IRunnable::run(), once per tick):
//   ENetSourceProvider (ISourceProvider) → NetworkEventParser (IParser)
//   → QueuedResourceProvider drains all events per tick
//   → ServerNetwork publishes each to ISubscriber<ServerNetworkEvent>
//
// Send path: sendTo / broadcast delegate directly to ENetSourceProvider.
//
// Active phases subscribe via ISubscriber<ServerNetworkEvent> RAII.
// In a future multithreaded phase, run() can be moved to its own thread.
class ServerNetwork : public Utils::PublishSubscribe::IPublisher<events::ServerNetworkEvent>,
                      public Utils::Runnables::IRunnable {
   public:
    ServerNetwork();
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
    void broadcastGameStart(const core::protocol::GameStartPacket &templatePkt,
                            const std::array<core::PlayerId, core::maxPlayers> &playerIds, uint8_t count);
    void broadcastSnapshot(const core::protocol::GameSnapshotPacket &packet);
    void broadcastRoundEnd(const core::protocol::RoundEndPacket &packet);
    void broadcastShutdown(const core::protocol::ServerShutdownPacket &packet);

    // IRunnable — drains ENet, parses events, publishes to subscribers.
    // Called once per server tick from ServerApp.
    void run() override;

   private:
    using EventProvider =
        Utils::Providers::QueuedResourceProvider<events::ServerNetworkEvent, core::network::RawNetworkMessage>;

    std::unique_ptr<EventProvider> m_eventProvider;
    core::network::ENetSourceProvider *m_enetSource = nullptr;  // non-owning; owned by m_eventProvider

    Utils::Logging::Logger m_logger{"ServerNetwork"};
};

}  // namespace pacman::server::network
