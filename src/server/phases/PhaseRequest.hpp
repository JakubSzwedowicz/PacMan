#pragma once

#include <array>
#include <cstdint>
#include <variant>

#include "core/Common.hpp"
#include "core/maps/Map.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::server::phases {

// Returned by Phase::update() to signal what should happen next.
// GameRunner inspects the result and performs the transition.

struct PhaseRunning {};  // keep running, no transition

struct StartGameRequest {
    core::maps::Map map;
    std::array<core::protocol::PlayerInfo, core::maxPlayers> players;
    uint8_t playerCount;
};

struct ReturnToLobbyRequest {};

using PhaseRequest = std::variant<PhaseRunning, StartGameRequest, ReturnToLobbyRequest>;

}  // namespace pacman::server::phases
