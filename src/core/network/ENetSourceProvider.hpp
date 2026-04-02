#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/Providers/ISourceProvider.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <span>
#include <string>

#include "core/network/RawNetworkMessage.hpp"

namespace pacman::core::network {

// Wraps an ENet host as an ISourceProvider<RawNetworkMessage>:
//   run()  — calls enet_host_service in a loop, queuing all pending events
//   poll() — dequeues one event at a time (FIFO), returns nullopt when empty
//
// Supports both server mode (startServer) and client mode (connectToServer).
// Outgoing traffic (sendTo / broadcast) is separate from the source-provider
// interface since ISourceProvider models the receive direction only.
class ENetSourceProvider : public Utils::Providers::ISourceProvider<RawNetworkMessage> {
   public:
    ENetSourceProvider();
    ~ENetSourceProvider() override;

    ENetSourceProvider(const ENetSourceProvider &) = delete;
    ENetSourceProvider &operator=(const ENetSourceProvider &) = delete;

    // Server mode: creates a host bound to the given port.
    [[nodiscard]] bool startServer(uint16_t port, int maxClients);

    // Client mode: creates a host and initiates a connection.
    // Blocks briefly to wait for the connect event (~5 s timeout).
    [[nodiscard]] bool connectToServer(const std::string &host, uint16_t port);

    // Tears down the ENet host regardless of mode.
    void stop();

    [[nodiscard]] bool isActive() const;

    // Returns the port the ENet host is actually bound to.
    // For server mode with port=0 this is the OS-assigned ephemeral port.
    [[nodiscard]] uint16_t boundPort() const;

    // ISourceProvider
    void run() override;
    [[nodiscard]] std::optional<RawNetworkMessage> poll() override;

    // Outgoing
    void sendTo(uint32_t peerId, std::span<const std::byte> data, bool reliable);
    void broadcast(std::span<const std::byte> data, bool reliable);

   private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    std::queue<RawNetworkMessage> m_queue;
    Utils::Logging::Logger m_logger{"ENetSourceProvider"};
};

}  // namespace pacman::core::network
