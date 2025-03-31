//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include <chrono>
#include <vector>

#include "Entities/EntitiesStates.h"
#include "Entities/Entity.h"

namespace PacMan::GameEvents {
using namespace GameObjects;
using namespace std::chrono_literals;
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
  ENTITY_MOVED_EVENT,
  ENTITY_AT_JUNCTION_EVENT,
  PELLET_EATEN_EVENT,
  POWER_PELLET_EATEN_EVENT,
  POWER_PELLET_EXPIRED_EVENT,
  GHOST_EATEN_EVENT,
  PLAYER_DIED_EVENT,
  PLAYER_RESPAWNED_EVENT,
  GHOST_STATE_CHANGED_EVENT,
  SCORED_UPDATED_EVENT
};

inline std::string toString(const EntityEventType type) {
  switch (type) {
  case EntityEventType::ENTITY_MOVED_EVENT:
    return "EntityMoved";
  case EntityEventType::ENTITY_AT_JUNCTION_EVENT:
    return "EntityAtJunction";
  case EntityEventType::PELLET_EATEN_EVENT:
    return "PelletEaten";
  case EntityEventType::POWER_PELLET_EATEN_EVENT:
    return "PowerPelletEaten";
  case EntityEventType::POWER_PELLET_EXPIRED_EVENT:
    return "PowerPelletExpired";
  case EntityEventType::GHOST_EATEN_EVENT:
    return "GhostEaten";
  case EntityEventType::PLAYER_DIED_EVENT:
    return "PlayerDied";
  case EntityEventType::PLAYER_RESPAWNED_EVENT:
    return "PlayerRespawned";
  case EntityEventType::GHOST_STATE_CHANGED_EVENT:
    return "GhostStateChanged";
  case EntityEventType::SCORED_UPDATED_EVENT:
    return "ScoreUpdatedEvent";
  }
  return "Unknown: " +
         std::to_string(
             static_cast<std::underlying_type_t<EntityEventType>>(type));
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
struct EntityMovedEvent final : public EntityEvent {
  Entities::TilePosition previousPosition;
  Entities::TilePosition newPosition;

  EntityMovedEvent(Entities::EntityId id, Entities::TilePosition prevPos,
              Entities::TilePosition newPos)
      : EntityEvent(id, EntityEventType::ENTITY_MOVED_EVENT),
        previousPosition(prevPos), newPosition(newPos) {}
};

/**
 * @brief Event published when an entity moves to a new tile.
 */
struct EntityAtJunctionEvent final : public EntityEvent {
  Entities::TilePosition junctionPosition;
  const std::vector<Entities::TilePosition> &adjacentNodes;

  EntityAtJunctionEvent(Entities::EntityId id,
                   Entities::TilePosition junctionPosition,
                   const std::vector<Entities::TilePosition> &adjacentNodes)
      : EntityEvent(id, EntityEventType::ENTITY_AT_JUNCTION_EVENT),
        junctionPosition(junctionPosition), adjacentNodes(adjacentNodes) {}
};

//-----------------------------------------------------------------------------
// Player Specific Events (Inherit from EntityEvent)
//-----------------------------------------------------------------------------

/**
 * @brief Event published when PacMan eats a standard pellet.
 */
struct PelletEatenEvent final : public EntityEvent {
  Entities::TilePosition position;
  uint32_t scoreValue;

  PelletEatenEvent(Entities::EntityId pacManId, Entities::TilePosition pos,
              uint32_t score)
      : EntityEvent(pacManId, EntityEventType::PELLET_EATEN_EVENT), position(pos),
        scoreValue(score) {}
};

/**
 * @brief Event published when PacMan eats a power pellet.
 * This often triggers other effects (like ghosts becoming frightened).
 */
struct PowerPelletEatenEvent final : public EntityEvent {
  Entities::TilePosition position;
  uint32_t scoreValue;
  std::chrono::milliseconds frightenDuration;

  PowerPelletEatenEvent(Entities::EntityId pacManId, Entities::TilePosition pos,
                   uint32_t score,
                   std::chrono::milliseconds frightenDuration = 5000ms)
      : EntityEvent(pacManId, EntityEventType::POWER_PELLET_EATEN_EVENT),
        position(pos), scoreValue(score), frightenDuration(frightenDuration) {}
};

/**
 * @brief Event published when PacMan stops being empowered.
 */
struct PowerPelletExpiredEvent final : public EntityEvent {
  explicit PowerPelletExpiredEvent(Entities::EntityId pacManId)
      : EntityEvent(pacManId, EntityEventType::POWER_PELLET_EXPIRED_EVENT) {}
};

/**
 * @brief Event published when PacMan eats a frightened ghost.
 */
struct GhostEatenEvent final : public EntityEvent {
  Entities::EntityId eatenGhostId;
  Entities::TilePosition position;
  uint32_t scoreValue;

  GhostEatenEvent(Entities::EntityId pacManId, Entities::EntityId ghostId,
             Entities::TilePosition pos, uint32_t score)
      : EntityEvent(pacManId, EntityEventType::GHOST_EATEN_EVENT),
        eatenGhostId(ghostId), position(pos), scoreValue(score) {}
};

/**
 * @brief Event published when PacMan loses a life and the death animation
 * starts.
 */
struct PlayerDiedEvent final : public EntityEvent {
  Entities::EntityId ghostId;
  Entities::TilePosition position;

  explicit PlayerDiedEvent(Entities::EntityId pacManId, Entities::EntityId ghostId,
                      Entities::TilePosition pos)
      : EntityEvent(pacManId, EntityEventType::PLAYER_DIED_EVENT), ghostId(ghostId),
        position(pos) {}
};

/**
 * @brief Event published when PacMan respawns after losing a life.
 */
struct PlayerRespawnedEvent final : public EntityEvent {
  Entities::TilePosition spawnPosition;
  explicit PlayerRespawnedEvent(Entities::EntityId pacManId,
                           Entities::TilePosition spawnPos)
      : EntityEvent(pacManId, EntityEventType::PLAYER_RESPAWNED_EVENT),
        spawnPosition(spawnPos) {}
};

/**
 * @brief Event published whenever the player's score changes.
 * Alternatively, score changes can be deduced by subscribing to
 * PelletEaten, GhostEaten, FruitEaten events. Publishing this explicitly
 * can simplify score display logic
 */
struct ScoreUpdatedEvent final : public EntityEvent {
  uint32_t newTotalScore;
  int changeAmount; // How much the score changed by

  ScoreUpdatedEvent(Entities::EntityId entityId, uint32_t newTotalScore,
                    int changeAmount)
      : EntityEvent(entityId, EntityEventType::SCORED_UPDATED_EVENT),
        newTotalScore(newTotalScore), changeAmount(changeAmount) {}
};

//-----------------------------------------------------------------------------
// Ghost Specific Events (Inherit from EntityEvent)
//-----------------------------------------------------------------------------

/**
 * @brief Event published when a ghost changes its behavioral state.
 */
struct GhostStateChangedEvent final : public EntityEvent {
  Entities::GhostState newState;
  Entities::GhostState previousState;

  GhostStateChangedEvent(Entities::EntityId ghostId, Entities::GhostState prev,
                    Entities::GhostState next)
      : EntityEvent(ghostId, EntityEventType::GHOST_STATE_CHANGED_EVENT),
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
struct GameEvent : public BaseEvent {};

/**
 * @brief Event published when a new level begins.
 */
struct GameStartedEvent final : public GameEvent {
  int levelNumber;

  explicit GameStartedEvent(int level) : levelNumber(level) {}
};

/**
 * @brief Event published when all pellets on a level are cleared.
 */
struct GameCompletedEvent final : public GameEvent {
  int levelNumber;

  explicit GameCompletedEvent(int level) : levelNumber(level) {}
};

/**
 * @brief Event published when the player runs out of lives.
 */
struct GameOverEvent final : public GameEvent {};

} // namespace PacMan::GameEvents

#endif // GAMEEVENTS_H
