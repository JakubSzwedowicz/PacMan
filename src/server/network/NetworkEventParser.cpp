#include "server/network/NetworkEventParser.hpp"

namespace pacman::server::network {

using RawType = core::network::RawNetworkMessage::Type;

events::ServerNetworkEvent NetworkEventParser::parse(core::network::RawNetworkMessage raw) {
    switch (raw.type) {
        case RawType::PeerConnected:
            return events::PlayerConnectedEvent{raw.peerId};

        case RawType::PeerDisconnected:
            return events::PlayerDisconnectedEvent{raw.peerId};

        case RawType::Packet:
            // TODO (Phase 4): inspect "type" discriminator in raw.data, deserialize via Glaze.
            // e.g.:
            //   auto json = std::string(reinterpret_cast<char*>(raw.data.data()), raw.data.size());
            //   std::string type = glz::get_value<std::string>(json, "/type").value_or("");
            //   if (type == "LobbyReady") return events::LobbyReadyEvent{...};
            //   if (type == "PlayerInput") return events::PlayerInputEvent{...};
            return events::UnknownPacketEvent{};
    }
    return events::UnknownPacketEvent{};
}

}  // namespace pacman::server::network
