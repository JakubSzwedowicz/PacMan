#pragma once

#include "server/game/GameSettings.hpp"
#include "server/network/ServerNetwork.hpp"
#include "server/phases/Phase.hpp"
#include "server/phases/PhaseRequest.hpp"

#include <Utils/Logging/LoggerSubscribed.h>

#include <memory>

namespace pacman::server::game {

// Owns the server-side phase FSM for a single game session.
// Settings are frozen at construction — config hot-reload does not affect a
// running session. Long-term, multiple GameRunner instances can run in parallel.
class GameRunner {
public:
    GameRunner(GameSettings settings, network::ServerNetwork &network);

    // Called each server tick. Runs the active phase; handles phase transitions.
    void update(float dt);

    // Cleanly exits the current phase. Call before destroying the runner.
    void shutdown();

private:
    void handleRequest(phases::PhaseRequest req);
    void transition(std::unique_ptr<phases::Phase> newPhase);

    GameSettings m_settings;
    network::ServerNetwork &m_network;
    std::unique_ptr<phases::Phase> m_currentPhase;
    std::unique_ptr<phases::Phase> m_pendingPhase;

    Utils::Logging::LoggerSubscribed m_logger{"GameRunner"};
};

} // namespace pacman::server::game
