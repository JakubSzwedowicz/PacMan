#pragma once

#include <Utils/Logging/LoggerSubscribed.h>
#include <Utils/Providers/QueuedResourceProvider.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>
#include <Utils/Runnables/IRunnable.h>

#include <memory>
#include <string>

#include "client/network/ClientNetworkEvents.hpp"
#include "core/network/ENetSourceProvider.hpp"
#include "core/network/RawNetworkMessage.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::network {

// Bridges ENet ↔ pub/sub, mirroring ServerNetwork on the client side.
//
// Receive path (IRunnable::run(), once per frame):
//   ENetSourceProvider (ISourceProvider) → ClientNetworkEventParser (IParser)
//   → QueuedResourceProvider drains all events per frame
//   → ClientNetwork publishes each to ISubscriber<ClientNetworkEvent>
//
// Send path: sendLobbyReady / sendReadyToPlay / sendInput delegate to
// ENetSourceProvider::sendTo(serverPeerId, ...).
//
// Active screens subscribe via ISubscriber<ClientNetworkEvent> RAII.
// In a future multithreaded phase, run() can be moved to its own thread.
class ClientNetwork : public Utils::PublishSubscribe::IPublisher<events::ClientNetworkEvent>,
                      public Utils::Runnables::IRunnable {
   public:
    ClientNetwork();
    ~ClientNetwork();

    ClientNetwork(const ClientNetwork &) = delete;
    ClientNetwork &operator=(const ClientNetwork &) = delete;
    ClientNetwork(ClientNetwork &&) = delete;
    ClientNetwork &operator=(ClientNetwork &&) = delete;

    // Blocks briefly to wait for the connect event (~5 s timeout).
    [[nodiscard]] bool connect(const std::string &host, uint16_t port);
    void disconnect();
    [[nodiscard]] bool isConnected() const;

    // Outgoing messages (implementations added in Phase 4)
    void sendLobbyReady(bool ready);
    void sendReadyToPlay();
    void sendInput(const core::protocol::PlayerInputPacket &packet);

    // IRunnable — drains ENet, parses events, publishes to subscribers.
    // Called once per frame from ClientApp.
    void run() override;

   private:
    using EventProvider =
        Utils::Providers::QueuedResourceProvider<events::ClientNetworkEvent, core::network::RawNetworkMessage>;

    std::unique_ptr<EventProvider> m_eventProvider;
    core::network::ENetSourceProvider *m_enetSource = nullptr;  // non-owning; owned by m_eventProvider

    Utils::Logging::LoggerSubscribed m_logger{"ClientNetwork"};
};

}  // namespace pacman::client::network
