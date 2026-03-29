#include "client/network/ClientNetworkEventParser.hpp"

#include "core/protocol/PacketCodec.hpp"

namespace pacman::client::network {

using core::network::RawNetworkMessage;
using namespace events;
using Type = core::protocol::PacketCodec::PacketType;

ClientNetworkEvent ClientNetworkEventParser::parse(RawNetworkMessage raw) {
    switch (raw.type) {
        case RawNetworkMessage::Type::PeerConnected:
            // The assigned PlayerId arrives later via GameStartPacket.assignedPlayerId.
            return ConnectedEvent{0};

        case RawNetworkMessage::Type::PeerDisconnected:
            return DisconnectedEvent{};

        case RawNetworkMessage::Type::Packet: {
            std::span<const std::byte> data{raw.data};
            switch (core::protocol::PacketCodec::getType(data)) {
                case Type::LobbyState: {
                    auto p = core::protocol::PacketCodec::deserializeLobbyState(data);
                    if (p) return LobbyStateEvent{*p};
                    break;
                }
                case Type::GameStart: {
                    auto p = core::protocol::PacketCodec::deserializeGameStart(data);
                    if (p) return GameStartEvent{*p};
                    break;
                }
                case Type::GameSnapshot: {
                    auto p = core::protocol::PacketCodec::deserializeGameSnapshot(data);
                    if (p) return GameSnapshotEvent{*p};
                    break;
                }
                case Type::RoundEnd: {
                    auto p = core::protocol::PacketCodec::deserializeRoundEnd(data);
                    if (p) return RoundEndEvent{*p};
                    break;
                }
                case Type::ServerShutdown: {
                    auto p = core::protocol::PacketCodec::deserializeServerShutdown(data);
                    if (p) return ServerShutdownEvent{*p};
                    break;
                }
                default:
                    break;
            }
            return UnknownPacketEvent{};
        }
    }
    return UnknownPacketEvent{};
}

}  // namespace pacman::client::network
