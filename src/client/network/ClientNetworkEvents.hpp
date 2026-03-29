#pragma once

#include <variant>

#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::network::events {

// Events the client receives from the server.
// ClientNetwork publishes these; active screens subscribe.

struct ConnectedEvent {
    core::PlayerId assignedId;
};
struct DisconnectedEvent {};
struct LobbyStateEvent {
    core::protocol::LobbyStatePacket packet;
};
struct GameStartEvent {
    core::protocol::GameStartPacket packet;
};
struct GameSnapshotEvent {
    core::protocol::GameSnapshotPacket packet;
};
struct RoundEndEvent {
    core::protocol::RoundEndPacket packet;
};
struct ServerShutdownEvent {
    core::protocol::ServerShutdownPacket packet;
};

// Returned by ClientNetworkEventParser for unknown or malformed packets.
struct UnknownPacketEvent {};

using ClientNetworkEvent = std::variant<UnknownPacketEvent, ConnectedEvent, DisconnectedEvent, LobbyStateEvent,
                                        GameStartEvent, GameSnapshotEvent, RoundEndEvent, ServerShutdownEvent>;

}  // namespace pacman::client::network::events
