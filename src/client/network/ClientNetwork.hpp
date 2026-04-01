#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>
#include <Utils/Runnables/IRunnable.h>

#include <string>

#include "client/network/ClientNetworkEventParser.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "core/network/ENetSourceProvider.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::network {

class ClientNetwork : public Utils::PublishSubscribe::IPublisher<events::ClientNetworkEvent>,
                      public Utils::Runnables::IRunnable {
   public:
    ClientNetwork() = default;
    ~ClientNetwork();

    ClientNetwork(const ClientNetwork&) = delete;
    ClientNetwork& operator=(const ClientNetwork&) = delete;
    ClientNetwork(ClientNetwork&&) = delete;
    ClientNetwork& operator=(ClientNetwork&&) = delete;

    void run() override;

    [[nodiscard]] bool connect(const std::string& host, uint16_t port);
    void disconnect();
    [[nodiscard]] bool isConnected() const;
    void sendLobbyReady(bool ready);
    void sendReadyToPlay();
    void sendInput(const core::protocol::PlayerInputPacket& packet);

   private:
    core::network::ENetSourceProvider m_enetSource;
    ClientNetworkEventParser m_parser;

    Utils::Logging::Logger m_logger{"ClientNetwork"};
};

}  // namespace pacman::client::network
