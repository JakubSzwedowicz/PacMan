#pragma once

#include "core/Common.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/Map.hpp"

#include <array>
#include <string>

// All packet types that cross the client↔server boundary.
// Serialized with FlatBuffers at the network layer; the structs here are the
// in-memory representation both sides operate on.

namespace pacman::core::protocol {

// ---------------------------------------------------------------------------
// Shared sub-types
// ---------------------------------------------------------------------------

struct PlayerInfo {
  PlayerId id = 0;
  std::string name;
  bool connected = false;
  bool ready = false;
};

struct EntityState {
  PlayerId id = 0;
  float x = 0.0f;
  float y = 0.0f;
  ecs::Direction dir = ecs::Direction::None;
  int score = 0;
  int lives = 0;
  bool alive = true;
};

struct GhostState {
  ecs::GhostType type = ecs::GhostType::Blinky;
  float x = 0.0f;
  float y = 0.0f;
  ecs::Direction dir = ecs::Direction::None;
  uint8_t mode = 0; // core::ecs::GhostState::Mode
};

// ---------------------------------------------------------------------------
// Lobby packets
// ---------------------------------------------------------------------------

// Server → all clients: current lobby state, broadcast on every change.
struct LobbyStatePacket {
  std::array<PlayerInfo, maxPlayers> players{};
  uint8_t playerCount = 0;
};

// Client → server: player toggles ready status.
struct LobbyReadyPacket {
  PlayerId playerId = 0;
  bool ready = false;
};

// ---------------------------------------------------------------------------
// Game start / loading packets
// ---------------------------------------------------------------------------

// Server → all clients: game is starting.
struct GameStartPacket {
  std::string mapJson;
  std::array<maps::Tile, maxPlayers> spawnPositions{};
  std::array<PlayerId, maxPlayers> playerIds{};
  uint8_t playerCount = 0;
  PlayerId assignedPlayerId = 0; // filled in per-client by the server
};

// Client → server: finished loading, ready for first tick.
struct ReadyToPlayPacket {
  PlayerId playerId = 0;
};

// ---------------------------------------------------------------------------
// In-game packets
// ---------------------------------------------------------------------------

// Client → server: one input sample per tick.
struct PlayerInputPacket {
  Tick tick = 0;
  PlayerId playerId = 0;
  ecs::Direction dir = ecs::Direction::None;
};

// Server → all clients: authoritative world state (sent at ~20 Hz).
struct GameSnapshotPacket {
  Tick tick = 0;
  std::array<EntityState, maxPlayers> players{};
  std::array<GhostState, ghostCount> ghosts{};
  uint8_t playerCount = 0;
};

// ---------------------------------------------------------------------------
// Round-end / shutdown packets
// ---------------------------------------------------------------------------

// Server → all clients: round finished.
struct RoundEndPacket {
  std::array<EntityState, maxPlayers> finalScores{};
  uint8_t playerCount = 0;
  PlayerId winnerId = 0; // player with the highest score
};

// Server → all clients: server is shutting down (host quit, crash, etc.).
struct ServerShutdownPacket {
  std::string reason;
};

} // namespace pacman::core::protocol
