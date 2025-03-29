//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "Entities/Entity.h"
#include "Entities/Ghost.h"

#include <chrono>

namespace PacMan::GameEvents {
using namespace GameObjects;

//=============================================================================
// Base Event Structure
//=============================================================================

/**
 * @brief Base struct for all game events.
 * Provides a common type and virtual destructor for safe polymorphism.
 */
struct BaseEvent {
  std::chrono::time_point<std::chrono::steady_clock> timestamp =
      std::chrono::steady_clock::now();
  virtual ~BaseEvent() = default;
};

//=============================================================================
// Entity Related Events
//=============================================================================

enum class EntityEventType {
  ENTITY_MOVED,
  PELLET_EATEN,
  POWER_PELLET_EATEN,
  GHOST_EATEN,
  PLAYER_CAUGHT,
  PLAYER_DIED,
  PLAYER_RESPAWNED,
  GHOST_STATE_CHANGED
};

std::string toString(const EntityEventType type) {
  switch (type) {
      case EntityEventType::ENTITY_MOVED:
    return "EntityMoved";
  case EntityEventType::PELLET_EATEN:
    return "PelletEaten";
  case EntityEventType::POWER_PELLET_EATEN:
    return "PowerPelletEaten";
  case EntityEventType::GHOST_EATEN:
    return "GhostEaten";
  case EntityEventType::PLAYER_CAUGHT:
    return "PlayerCaught";
  case EntityEventType::PLAYER_DIED:
    return "PlayerDied";
  case EntityEventType::PLAYER_RESPAWNED:
    return "PlayerRespawned";
  case EntityEventType::GHOST_STATE_CHANGED:
    return "GhostStateChanged";
  }
  return "Unknown";
}

/**
 * @brief Base struct for events related to a specific game entity.
 */
struct EntityEvent : public BaseEvent {
  Entities::EntityId entityId;
  EntityEventType eventType;
  explicit EntityEvent(const Entities::EntityId id,
                       const EntityEventType eventType)
      : entityId(id), eventType(eventType) {}
  [[nodiscard]] std::string toString() const {
    return "{EntityEventId: " + std::to_string(entityId) +
           ", EntityEventType: " + GameEvents::toString(eventType) + "}";
  }
};

/**
 * @brief Event published when an entity moves to a new tile.
 */
struct EntityMoved final : public EntityEvent {
  Entities::TilePosition previousPosition;
  Entities::TilePosition newPosition;

  EntityMoved(Entities::EntityId id, Entities::TilePosition prevPos,
              Entities::TilePosition newPos)
      : EntityEvent(id, EntityEventType::ENTITY_MOVED),
        previousPosition(prevPos), newPosition(newPos) {}
};

//-----------------------------------------------------------------------------
// Player Specific Events (Inherit from EntityEvent)
//-----------------------------------------------------------------------------

/**
 * @brief Event published when PacMan eats a standard pellet.
 */
struct PelletEaten final : public EntityEvent {
  Entities::TilePosition position;
  uint32_t scoreValue;

  PelletEaten(Entities::EntityId pacManId, Entities::TilePosition pos,
              uint32_t score)
      : EntityEvent(pacManId, EntityEventType::PELLET_EATEN), position(pos),
        scoreValue(score) {}
};

/**
 * @brief Event published when PacMan eats a power pellet.
 * This often triggers other effects (like ghosts becoming frightened).
 */
struct PowerPelletEaten final : public EntityEvent {
  Entities::TilePosition position;
  uint32_t scoreValue;
  // std::chrono::milliseconds frightenDuration{5000};

  PowerPelletEaten(Entities::EntityId pacManId, Entities::TilePosition pos,
                   uint32_t score)
      : EntityEvent(pacManId, EntityEventType::POWER_PELLET_EATEN),
        position(pos), scoreValue(score) {}
};

/**
 * @brief Event published when PacMan eats a frightened ghost.
 */
struct GhostEaten final : public EntityEvent {
  Entities::EntityId eatenGhostId;
  Entities::TilePosition position;
  uint32_t scoreValue;

  GhostEaten(Entities::EntityId pacManId, Entities::EntityId ghostId,
             Entities::TilePosition pos, uint32_t score)
      : EntityEvent(pacManId, EntityEventType::GHOST_EATEN),
        eatenGhostId(ghostId), position(pos), scoreValue(score) {}
};

/**
 * @brief Event published when PacMan collides with a non-frightened ghost.
 */
struct PlayerCaught final : public EntityEvent {
  Entities::EntityId ghostId;
  Entities::TilePosition position;

  PlayerCaught(Entities::EntityId pacManId, Entities::EntityId ghostId,
               Entities::TilePosition pos)
      : EntityEvent(pacManId, EntityEventType::PLAYER_CAUGHT), ghostId(ghostId),
        position(pos) {}
};

/**
 * @brief Event published when PacMan loses a life and the death animation
 * starts.
 */
struct PlayerDied final : public EntityEvent {
  explicit PlayerDied(Entities::EntityId pacManId)
      : EntityEvent(pacManId, EntityEventType::PLAYER_DIED) {}
};

/**
 * @brief Event published when PacMan respawns after losing a life.
 */
struct PlayerRespawned final : public EntityEvent {
  Entities::TilePosition spawnPosition;
  explicit PlayerRespawned(Entities::EntityId pacManId,
                           Entities::TilePosition spawnPos)
      : EntityEvent(pacManId, EntityEventType::PLAYER_RESPAWNED),
        spawnPosition(spawnPos) {}
};

//-----------------------------------------------------------------------------
// Ghost Specific Events (Inherit from EntityEvent)
//-----------------------------------------------------------------------------

/**
 * @brief Event published when a ghost changes its behavioral state.
 */
struct GhostStateChanged final : public EntityEvent {
  Entities::GhostState newState;
  Entities::GhostState previousState;

  GhostStateChanged(Entities::EntityId ghostId, Entities::GhostState prev,
                    Entities::GhostState next)
      : EntityEvent(ghostId, EntityEventType::GHOST_STATE_CHANGED),
        newState(next), previousState(prev) {}
};

/**
 * @brief Event published when a ghost leaves the central pen.
 */
// struct GhostLeftPen final : public EntityEvent {
//   explicit GhostLeftPen(Entities::EntityId ghostId) : EntityEvent(ghostId) {}
// };

/**
 * @brief Event published when an eaten ghost returns to the central pen.
 */
// struct GhostReturnedToPen final : public EntityEvent {
//   explicit GhostReturnedToPen(Entities::EntityId ghostId) :
//   EntityEvent(ghostId) {}
// };

//=============================================================================
// Gameplay State Events (Inherit directly from BaseEvent)
//=============================================================================

/**
 * @brief Event published when the game starts for the first time or after a
 * reset.
 */
struct GameStarted final : public BaseEvent {};

/**
 * @brief Event published when a new level begins.
 */
struct LevelStarted final : public BaseEvent {
  int levelNumber;

  explicit LevelStarted(int level) : levelNumber(level) {}
};

/**
 * @brief Event published when all pellets on a level are cleared.
 */
struct LevelCompleted final : public BaseEvent {
  int levelNumber;

  explicit LevelCompleted(int level) : levelNumber(level) {}
};

/**
 * @brief Event published when the player runs out of lives.
 */
struct GameOver final : public BaseEvent {};

/**
 * @brief Event published whenever the player's score changes.
 * Alternatively, score changes can be deduced by subscribing to
 * PelletEaten, GhostEaten, FruitEaten events. Publishing this explicitly
 * can simplify score display logic.
 */
struct ScoreUpdated final : public BaseEvent {
  int newTotalScore;
  int changeAmount; // How much the score changed by

  ScoreUpdated(int total, int change)
      : newTotalScore(total), changeAmount(change) {}
};

/**
 * @brief Event published when the number of lives remaining changes.
 */
struct LivesChanged final : public BaseEvent {
  int remainingLives;

  explicit LivesChanged(int lives) : remainingLives(lives) {}
};

} // namespace PacMan::GameEvents

#endif // GAMEEVENTS_H
