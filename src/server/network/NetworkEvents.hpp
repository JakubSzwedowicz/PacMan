#pragma once

#include <variant>

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::server::network::events {

struct PlayerConnectedEvent {
    core::PlayerId playerId;
};
struct PlayerDisconnectedEvent {
    core::PlayerId playerId;
};
struct LobbyReadyEvent {
    core::PlayerId playerId;
    bool ready;
};
struct ReadyToPlayEvent {
    core::PlayerId playerId;
};
struct PlayerInputEvent {
    core::protocol::PlayerInputPacket packet;
};

// Returned by NetworkEventParser for unknown or malformed packets.
// Published to subscribers but silently ignored by all phases.
struct UnknownPacketEvent {};

// All network events the server can receive from clients.
// ServerNetwork publishes these; active phases subscribe.
using ServerNetworkEvent = std::variant<UnknownPacketEvent, PlayerConnectedEvent, PlayerDisconnectedEvent,
                                        LobbyReadyEvent, ReadyToPlayEvent, PlayerInputEvent>;

}  // namespace pacman::server::network::events
