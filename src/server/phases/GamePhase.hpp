#pragma once

#include "server/ai/AISystem.hpp"
#include "server/network/NetworkEvents.hpp"
#include "server/network/ServerNetwork.hpp"
#include "server/phases/AuthoritativeLogic.hpp"
#include "server/phases/Phase.hpp"
#include "server/render/AsciiRenderer.hpp"

#include "core/maps/Map.hpp"
#include "core/protocol/Packets.hpp"
#include "core/simulation/Simulation.hpp"

#include <Utils/Logging/LoggerSubscribed.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <entt/entt.hpp>

#include <array>
#include <optional>
#include <unordered_map>

namespace pacman::server::phases {

class GamePhase
    : public Phase,
      public Utils::PublishSubscribe::ISubscriber<network::events::ServerNetworkEvent> {
public:
    GamePhase(network::ServerNetwork &network,
              core::maps::Map map,
              std::array<core::protocol::PlayerInfo, core::maxPlayers> players,
              uint8_t playerCount,
              bool renderAscii,
              int renderIntervalMs);

    // Phase
    void onEnter() override;
    void onExit() override;
    PhaseRequest update(float dt) override;

    // ISubscriber<ServerNetworkEvent>
    void onUpdate(const network::events::ServerNetworkEvent &event) override;

private:
    void handleDisconnect(core::PlayerId id);
    void handleInput(const core::protocol::PlayerInputPacket &packet);

    void spawnEntities();
    void applyPendingInputs();
    void broadcastSnapshot();
    [[nodiscard]] bool isRoundOver() const;
    void endRound();
    [[nodiscard]] core::protocol::GameSnapshotPacket buildSnapshot() const;
    [[nodiscard]] core::protocol::RoundEndPacket buildRoundEnd() const;

    network::ServerNetwork &m_network;
    core::maps::Map m_map;

    // Set by endRound(); drained by update().
    std::optional<PhaseRequest> m_pendingRequest;

    entt::registry m_registry;
    core::simulation::Simulation m_simulation;
    ai::AISystem m_aiSystem;
    AuthoritativeLogic m_rules;
    render::AsciiRenderer m_asciiRenderer;

    std::array<core::protocol::PlayerInfo, core::maxPlayers> m_players{};
    uint8_t m_playerCount = 0;
    std::unordered_map<core::PlayerId, entt::entity> m_playerEntities;
    std::unordered_map<core::PlayerId, core::protocol::PlayerInputPacket> m_pendingInputs;

    core::Tick m_tick = 0;
    float m_snapshotAccumulator = 0.0f;
    static constexpr float snapshotRate = 1.0f / 20.0f;

    bool m_renderAscii = false;
    float m_renderInterval = 0.5f;
    float m_renderAccumulator = 0.0f;

    Utils::Logging::LoggerSubscribed m_logger{"GamePhase"};
};

} // namespace pacman::server::phases
