#include "client/network/ClientNetworkEventParser.hpp"

namespace pacman::client::network {

using core::network::RawNetworkMessage;
using namespace events;

ClientNetworkEvent ClientNetworkEventParser::parse(RawNetworkMessage raw) {
    switch (raw.type) {
        case RawNetworkMessage::Type::PeerConnected:
            // TODO (Phase 4): server sends assigned PlayerId in a connect packet.
            // For now, assign 0 as a placeholder.
            return ConnectedEvent{0};

        case RawNetworkMessage::Type::PeerDisconnected:
            return DisconnectedEvent{};

        case RawNetworkMessage::Type::Packet:
            // TODO (Phase 4): deserialize FlatBuffers packet, dispatch by type tag.
            //   switch (packetType) {
            //   case PacketType::LobbyState:   return LobbyStateEvent{...};
            //   case PacketType::GameStart:    return GameStartEvent{...};
            //   case PacketType::GameSnapshot: return GameSnapshotEvent{...};
            //   case PacketType::RoundEnd:     return RoundEndEvent{...};
            //   case PacketType::Shutdown:     return ServerShutdownEvent{...};
            //   }
            return UnknownPacketEvent{};
    }
    return UnknownPacketEvent{};
}

}  // namespace pacman::client::network
