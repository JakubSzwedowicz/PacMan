#pragma once

#include <string>

namespace pacman::server::game {

// Snapshot of server config taken at the start of a game session.
// Frozen for the session's lifetime — hot-reload of the config file
// does not affect a running game.
struct GameSettings {
    std::string mapPath;
    int maxPlayers = 4;
    bool renderAscii = false;
    int renderIntervalMs = 500;
};

} // namespace pacman::server::game
