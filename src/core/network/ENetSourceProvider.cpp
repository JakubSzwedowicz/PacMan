#include "core/network/ENetSourceProvider.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <enet/enet.h>

#include <cstring>
#include <unordered_map>

namespace pacman::core::network {

struct ENetSourceProvider::Impl {
    bool active = false;
    ENetHost *host = nullptr;
    ENetPeer *serverPeer = nullptr;  // client mode only
    std::unordered_map<uint32_t, ENetPeer *> peers;
    uint32_t nextPeerId = 1;
};

ENetSourceProvider::ENetSourceProvider() : m_impl(std::make_unique<Impl>()) {}

ENetSourceProvider::~ENetSourceProvider() { stop(); }

bool ENetSourceProvider::startServer(uint16_t port, int maxClients) {
    m_queue = {};
    m_impl->peers.clear();
    m_impl->serverPeer = nullptr;
    m_impl->nextPeerId = 1;

    if (enet_initialize() != 0) {
        LOG_E("enet_initialize() failed");
        return false;
    }
    ENetAddress addr{};
    addr.host = ENET_HOST_ANY;
    addr.port = port;
    m_impl->host = enet_host_create(&addr, static_cast<size_t>(maxClients), 2, 0, 0);
    if (!m_impl->host) {
        LOG_E("enet_host_create() failed (port={}, maxClients={})", port, maxClients);
        enet_deinitialize();
        return false;
    }
    m_impl->active = true;
    LOG_I("ENet server listening on port {} (maxClients={})", port, maxClients);
    return true;
}

bool ENetSourceProvider::connectToServer(const std::string &host, uint16_t port) {
    m_queue = {};
    m_impl->peers.clear();
    m_impl->serverPeer = nullptr;
    m_impl->nextPeerId = 1;

    if (enet_initialize() != 0) {
        LOG_E("enet_initialize() failed");
        return false;
    }
    m_impl->host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!m_impl->host) {
        LOG_E("enet_host_create() failed (client mode)");
        enet_deinitialize();
        return false;
    }
    ENetAddress addr{};
    if (enet_address_set_host(&addr, host.c_str()) != 0) {
        LOG_E("enet_address_set_host() failed for '{}'", host);
        enet_host_destroy(m_impl->host);
        m_impl->host = nullptr;
        enet_deinitialize();
        return false;
    }
    addr.port = port;
    m_impl->serverPeer = enet_host_connect(m_impl->host, &addr, 2, 0);
    if (!m_impl->serverPeer) {
        LOG_E("enet_host_connect() failed");
        enet_host_destroy(m_impl->host);
        m_impl->host = nullptr;
        enet_deinitialize();
        return false;
    }
    ENetEvent event;
    if (enet_host_service(m_impl->host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        // In client mode we intentionally reset nextPeerId on every connect so
        // the connected server always occupies peerId=1 for the lifetime of
        // this session.
        uint32_t id = m_impl->nextPeerId++;
        m_impl->peers[id] = m_impl->serverPeer;
        m_impl->serverPeer->data = reinterpret_cast<void *>(static_cast<uintptr_t>(id));
        m_impl->active = true;
        LOG_I("Connected to {}:{} (peerId={})", host, port, id);
        return true;
    }
    LOG_E("Connection to {}:{} timed out or failed", host, port);
    enet_peer_reset(m_impl->serverPeer);
    m_impl->serverPeer = nullptr;
    enet_host_destroy(m_impl->host);
    m_impl->host = nullptr;
    enet_deinitialize();
    return false;
}

void ENetSourceProvider::stop() {
    if (!m_impl->active) return;
    m_impl->active = false;

    if (m_impl->host) {
        if (m_impl->serverPeer) {
            enet_peer_disconnect(m_impl->serverPeer, 0);

            ENetEvent event;
            bool disconnected = false;
            while (enet_host_service(m_impl->host, &event, 100) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_RECEIVE:
                        enet_packet_destroy(event.packet);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        if (event.peer == m_impl->serverPeer) disconnected = true;
                        break;
                    default:
                        break;
                }
                if (disconnected) break;
            }
            if (!disconnected) {
                enet_peer_reset(m_impl->serverPeer);
            }
        } else {
            for (auto &[id, peer] : m_impl->peers) {
                (void)id;
                enet_peer_disconnect(peer, 0);
            }
            enet_host_flush(m_impl->host);
        }
    }

    m_impl->peers.clear();
    if (m_impl->host) {
        enet_host_destroy(m_impl->host);
        m_impl->host = nullptr;
    }
    m_impl->serverPeer = nullptr;
    m_impl->nextPeerId = 1;
    m_queue = {};
    enet_deinitialize();
    LOG_I("ENetSourceProvider stopped");
}

bool ENetSourceProvider::isActive() const { return m_impl->active; }

void ENetSourceProvider::run() {
    if (!m_impl->active || !m_impl->host) return;

    ENetEvent event;
    while (enet_host_service(m_impl->host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                uint32_t id = m_impl->nextPeerId++;
                m_impl->peers[id] = event.peer;
                event.peer->data = reinterpret_cast<void *>(static_cast<uintptr_t>(id));
                LOG_I("Peer {} connected", id);
                m_queue.push({RawNetworkMessage::Type::PeerConnected, id, 0, {}});
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: {
                auto id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(event.peer->data));
                LOG_I("Peer {} disconnected", id);
                m_impl->peers.erase(id);
                event.peer->data = nullptr;
                m_queue.push({RawNetworkMessage::Type::PeerDisconnected, id, 0, {}});
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                auto id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(event.peer->data));
                RawNetworkMessage msg{RawNetworkMessage::Type::Packet, id, event.channelID, {}};
                msg.data.resize(event.packet->dataLength);
                std::memcpy(msg.data.data(), event.packet->data, event.packet->dataLength);
                m_queue.push(std::move(msg));
                enet_packet_destroy(event.packet);
                break;
            }
            default:
                break;
        }
    }
}

std::optional<RawNetworkMessage> ENetSourceProvider::poll() {
    if (m_queue.empty()) return std::nullopt;
    auto msg = std::move(m_queue.front());
    m_queue.pop();
    return msg;
}

void ENetSourceProvider::sendTo(uint32_t peerId, std::span<const std::byte> data, bool reliable) {
    if (!m_impl->active || !m_impl->host) return;
    auto it = m_impl->peers.find(peerId);
    if (it == m_impl->peers.end()) {
        LOG_W("sendTo: unknown peerId {}", peerId);
        return;
    }
    auto flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0u;
    auto *packet = enet_packet_create(reinterpret_cast<const void *>(data.data()), data.size(), flags);
    enet_peer_send(it->second, 0, packet);
    enet_host_flush(m_impl->host);
}

void ENetSourceProvider::broadcast(std::span<const std::byte> data, bool reliable) {
    if (!m_impl->active || !m_impl->host) return;
    auto flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0u;
    auto *packet = enet_packet_create(reinterpret_cast<const void *>(data.data()), data.size(), flags);
    enet_host_broadcast(m_impl->host, 0, packet);
    enet_host_flush(m_impl->host);
}

}  // namespace pacman::core::network
