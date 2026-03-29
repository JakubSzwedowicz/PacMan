#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace pacman::core::network {

// Raw ENet event before packet deserialization.
// Produced by ENetTransport and consumed by NetworkEventParser.
struct RawNetworkMessage {
    enum class Type : uint8_t { PeerConnected, PeerDisconnected, Packet };

    Type type;
    uint32_t peerId = 0;
    uint8_t channel = 0;
    std::vector<std::byte> data;  // populated only for Type::Packet
};

}  // namespace pacman::core::network
