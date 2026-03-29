#include "server/network/NetworkEventParser.hpp"

#include "core/protocol/PacketCodec.hpp"

namespace pacman::server::network {

using RawType = core::network::RawNetworkMessage::Type;
using Type = core::protocol::PacketCodec::PacketType;

events::ServerNetworkEvent NetworkEventParser::parse(core::network::RawNetworkMessage raw) {
    switch (raw.type) {
        case RawType::PeerConnected:
            return events::PlayerConnectedEvent{raw.peerId};

        case RawType::PeerDisconnected:
            return events::PlayerDisconnectedEvent{raw.peerId};

        case RawType::Packet: {
            std::span<const std::byte> data{raw.data};
            switch (core::protocol::PacketCodec::getType(data)) {
                case Type::LobbyReady: {
                    auto p = core::protocol::PacketCodec::deserializeLobbyReady(data);
                    if (p) return events::LobbyReadyEvent{raw.peerId, p->ready};
                    break;
                }
                case Type::ReadyToPlay:
                    if (core::protocol::PacketCodec::deserializeReadyToPlay(data))
                        return events::ReadyToPlayEvent{raw.peerId};
                    break;
                case Type::PlayerInput: {
                    auto p = core::protocol::PacketCodec::deserializePlayerInput(data);
                    if (p) return events::PlayerInputEvent{*p};
                    break;
                }
                default:
                    break;
            }
            return events::UnknownPacketEvent{};
        }
    }
    return events::UnknownPacketEvent{};
}

}  // namespace pacman::server::network
