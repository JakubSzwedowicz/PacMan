#pragma once

#include "client/network/ClientNetworkEvents.hpp"
#include "core/network/RawNetworkMessage.hpp"

#include <Utils/Providers/IParser.h>

namespace pacman::client::network {

// Converts a raw ENet message into a typed client network event.
// Returns UnknownPacketEvent for unrecognized or malformed packets.
class ClientNetworkEventParser
    : public Utils::Providers::IParser<core::network::RawNetworkMessage,
                                       events::ClientNetworkEvent> {
public:
    events::ClientNetworkEvent parse(core::network::RawNetworkMessage raw) override;
};

} // namespace pacman::client::network
