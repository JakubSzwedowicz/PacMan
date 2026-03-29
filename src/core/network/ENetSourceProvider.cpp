#include "core/network/ENetSourceProvider.hpp"

#include <Utils/Logging/LoggerMacros.h>

namespace pacman::core::network {

struct ENetSourceProvider::Impl {
    bool active = false;
    // TODO (Phase 4): ENetHost *host = nullptr;
    //   Server mode: ENetAddress addr; enet_host_create(&addr, maxClients, 2, 0, 0)
    //   Client mode: ENetHost *host = enet_host_create(nullptr, 1, 2, 0, 0)
    //                ENetPeer *serverPeer = nullptr; uint32_t serverPeerId = 1;
    //   Both:        std::unordered_map<uint32_t, ENetPeer *> peers;
    //                uint32_t nextPeerId = 1;
};

ENetSourceProvider::ENetSourceProvider() : m_impl(std::make_unique<Impl>()) {}
ENetSourceProvider::~ENetSourceProvider() { stop(); }

bool ENetSourceProvider::startServer(uint16_t port, int maxClients) {
    // TODO (Phase 4):
    //   if (enet_initialize() != 0) { LOG_E(...); return false; }
    //   ENetAddress addr{}; addr.host = ENET_HOST_ANY; addr.port = port;
    //   m_impl->host = enet_host_create(&addr, maxClients, 2, 0, 0);
    //   if (!m_impl->host) { LOG_E(...); enet_deinitialize(); return false; }
    LOG_I("ENetSourceProvider stub startServer (port={}, maxClients={})", port, maxClients);
    (void)port; (void)maxClients;
    m_impl->active = true;
    return true;
}

bool ENetSourceProvider::connectToServer(const std::string &host, uint16_t port) {
    // TODO (Phase 4):
    //   if (enet_initialize() != 0) { LOG_E(...); return false; }
    //   m_impl->host = enet_host_create(nullptr, 1, 2, 0, 0);
    //   if (!m_impl->host) { LOG_E(...); enet_deinitialize(); return false; }
    //   ENetAddress addr; enet_address_set_host(&addr, host.c_str()); addr.port = port;
    //   m_impl->serverPeer = enet_host_connect(m_impl->host, &addr, 2, 0);
    //   ENetEvent event;
    //   if (enet_host_service(m_impl->host, &event, 5000) > 0 &&
    //       event.type == ENET_EVENT_TYPE_CONNECT) {
    //       m_impl->active = true; return true;
    //   }
    //   enet_peer_reset(m_impl->serverPeer); return false;
    LOG_I("ENetSourceProvider stub connectToServer ({}:{})", host, port);
    (void)host; (void)port;
    m_impl->active = true;
    return true;
}

void ENetSourceProvider::stop() {
    if (!m_impl->active) return;
    // TODO (Phase 4): enet_host_destroy(m_impl->host); enet_deinitialize();
    m_impl->active = false;
}

bool ENetSourceProvider::isActive() const { return m_impl->active; }

void ENetSourceProvider::run() {
    if (!m_impl->active) return;
    // TODO (Phase 4): drain all pending ENet events into m_queue
    //   ENetEvent event;
    //   while (enet_host_service(m_impl->host, &event, 0) > 0) {
    //       switch (event.type) {
    //       case ENET_EVENT_TYPE_CONNECT: {
    //           uint32_t id = m_impl->nextPeerId++;
    //           m_impl->peers[id] = event.peer;
    //           event.peer->data = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
    //           m_queue.push({RawNetworkMessage::Type::PeerConnected, id});
    //           break;
    //       }
    //       case ENET_EVENT_TYPE_DISCONNECT: {
    //           auto id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(event.peer->data));
    //           m_impl->peers.erase(id);
    //           m_queue.push({RawNetworkMessage::Type::PeerDisconnected, id});
    //           break;
    //       }
    //       case ENET_EVENT_TYPE_RECEIVE: {
    //           auto id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(event.peer->data));
    //           RawNetworkMessage msg{RawNetworkMessage::Type::Packet, id, event.channelID, {}};
    //           msg.data.resize(event.packet->dataLength);
    //           std::memcpy(msg.data.data(), event.packet->data, event.packet->dataLength);
    //           m_queue.push(std::move(msg));
    //           enet_packet_destroy(event.packet);
    //           break;
    //       }
    //       default: break;
    //       }
    //   }
}

std::optional<RawNetworkMessage> ENetSourceProvider::poll() {
    if (m_queue.empty()) return std::nullopt;
    auto msg = std::move(m_queue.front());
    m_queue.pop();
    return msg;
}

void ENetSourceProvider::sendTo(uint32_t peerId, std::span<const std::byte> data,
                                 bool reliable) {
    if (!m_impl->active) return;
    // TODO (Phase 4): enet_peer_send + enet_host_flush
    (void)peerId; (void)data; (void)reliable;
}

void ENetSourceProvider::broadcast(std::span<const std::byte> data, bool reliable) {
    if (!m_impl->active) return;
    // TODO (Phase 4): enet_host_broadcast + enet_host_flush
    (void)data; (void)reliable;
}

} // namespace pacman::core::network
