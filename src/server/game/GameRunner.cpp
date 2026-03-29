#include "server/game/GameRunner.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include "core/Common.hpp"
#include "server/phases/GamePhase.hpp"
#include "server/phases/LobbyPhase.hpp"

namespace pacman::server::game {

GameRunner::GameRunner(GameSettings settings, network::ServerNetwork &network)
    : m_settings(std::move(settings)), m_network(network) {
    transition(std::make_unique<phases::LobbyPhase>(m_network, m_settings.mapPath, m_settings.maxPlayers));
}

void GameRunner::update(float dt) {
    // Apply any pending phase transition before stepping — ensures the new
    // phase is subscribed to network events before pumpConfigAndNetwork fires.
    if (m_pendingPhase) {
        if (m_currentPhase) m_currentPhase->onExit();
        m_currentPhase = std::move(m_pendingPhase);
        m_currentPhase->onEnter();
        LOG_I("Phase transition complete");
    }

    if (m_currentPhase) {
        handleRequest(m_currentPhase->update(dt));
    }
}

void GameRunner::shutdown() {
    if (m_currentPhase) {
        m_currentPhase->onExit();
        m_currentPhase.reset();
    }
}

void GameRunner::handleRequest(phases::PhaseRequest req) {
    std::visit(
        pacman::overloaded{
            [](const phases::PhaseRunning &) {},
            [this](phases::StartGameRequest &r) {
                transition(std::make_unique<phases::GamePhase>(m_network, std::move(r.map), r.players, r.playerCount,
                                                               m_settings.renderAscii, m_settings.renderIntervalMs));
            },
            [this](const phases::ReturnToLobbyRequest &) {
                transition(std::make_unique<phases::LobbyPhase>(m_network, m_settings.mapPath, m_settings.maxPlayers));
            },
        },
        req);
}

void GameRunner::transition(std::unique_ptr<phases::Phase> newPhase) { m_pendingPhase = std::move(newPhase); }

}  // namespace pacman::server::game
