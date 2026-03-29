#pragma once

#include "core/network/RawNetworkMessage.hpp"
#include "server/network/NetworkEvents.hpp"

#include <Utils/Providers/IParser.h>

namespace pacman::server::network {

// Converts a raw ENet message into a typed server network event.
// Returns UnknownPacketEvent for unrecognized or malformed packets.
class NetworkEventParser
    : public Utils::Providers::IParser<core::network::RawNetworkMessage,
                                       events::ServerNetworkEvent> {
public:
    events::ServerNetworkEvent parse(core::network::RawNetworkMessage raw) override;
};

} // namespace pacman::server::network
