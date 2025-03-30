//
// Created by Jakub Szwedowicz on 2/24/25.
//

#include "Entities/Ghost.h"
#include "GameObjects/Level.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

using namespace std::chrono_literals;
Ghost::Ghost(Level *level, GameEvents::GameEventsManager &gameEventsManager)
    : MovingEntity(EntityType::GHOST, level),
      ISubscriber(&gameEventsManager.getEntityEventPublisher()),
      m_logger(std::make_unique<Utils::Logger>("Ghost", Utils::LogLevel::INFO)),
      m_entityEventsPublisher(gameEventsManager.getEntityEventPublisher()) {
  m_logger->logInfo(toString() + " initialized and subscribed to events.");
}

void Ghost::update(std::chrono::milliseconds deltaTime) {
  if (m_ghostState == GhostState::FRIGHTENED) {
    m_frightenedDurationMs -= deltaTime;
    if (m_frightenedDurationMs <= 0ms) {
      m_logger->logInfo("Frightened timer expired");
      revertFromFrightenedState();
    }
  }
}

void Ghost::callback(const GameEvents::EntityEvent &event) {
  m_logger->logDebug("Callback received event: " + event.toString());

  switch (event.eventType) {
  case GameEvents::EntityEventType::POWER_PELLET_EATEN: {
    const auto &pelletEvent =
        static_cast<const GameEvents::PowerPelletEaten &>(event);
    m_logger->logDebug("Processing POWER_PELLET_EATEN event.");
    setFrightenedState(pelletEvent.frightenDuration);
    break;
  }

  case GameEvents::EntityEventType::POWER_PELLET_EXPIRED: {
    m_logger->logDebug("Processing POWER_PELLET_EXPIRED event.");
    // ghosts clears frightened state in update(...)
    break;
  }

  case GameEvents::EntityEventType::GHOST_EATEN: {
    const auto &eatenEvent = static_cast<const GameEvents::GhostEaten &>(event);
    if (eatenEvent.eatenGhostId == m_entityId) {
      m_logger->logDebug("Processing GHOST_EATEN event: " +
                         eatenEvent.toString());
      setDeadState();
    }
    break;
  }

    // --- Events Ghosts Might Log or Ignore ---
  case GameEvents::EntityEventType::ENTITY_MOVED: {
    if (event.entityId == m_entityId) {
      const auto &movedEvent =
          static_cast<const GameEvents::EntityMoved &>(event);
      m_logger->logDebug(
          "Processing own ENTITY_MOVED event: " + movedEvent.toString() +
          " To (" + std::to_string(movedEvent.newPosition.x) + "," +
          std::to_string(movedEvent.newPosition.y) + ")");
      // TODO: decide where to go!
    }
    break;
  }

  case GameEvents::EntityEventType::GHOST_STATE_CHANGED: {
    // Log own state change confirmation if needed
    if (event.entityId == m_entityId) {
      const auto &stateEvent =
          static_cast<const GameEvents::GhostStateChanged &>(event);
      m_logger->logDebug("Received confirmation of own state change to: " +
                         Entities::toString(stateEvent.newState));
    } else {
      // TODO: Do nothing here unless I decide on implementing some smarter AI
    }
    break;
  }

    // --- Events Ghosts Typically Ignore ---
  case GameEvents::EntityEventType::PELLET_EATEN: {
    break;
  }
  case GameEvents::EntityEventType::PLAYER_DIED: {
    // TODO: Maybe update target direction?
    //        On the other hand it will happen anyway once it reaches tile.
    break;
  }
  case GameEvents::EntityEventType::PLAYER_RESPAWNED:
    // Ghosts generally don't react directly to these events in their callback.
    // AI logic in update() might consider PacMan's state/position indirectly.
    m_logger->logDebug("Ignoring event type: " + event.toString());
    break;

  default:
    m_logger->logError("Received unhandled EntityEvent: " + event.toString());
    break;
  }
}

// --- Private Helper Methods ---
void Ghost::setGhostState(const GhostState newState) {
  if (m_ghostState == newState) {
    return; // No change
  }

  m_previousGhostState = m_ghostState;
  m_ghostState = newState;
  m_logger->logInfo("State changed from " +
                    Entities::toString(m_previousGhostState) + " to " +
                    Entities::toString(m_ghostState));

  // Publish the state change event
  m_entityEventsPublisher.publish(GameEvents::GhostStateChanged(
      m_entityId, m_previousGhostState, m_ghostState));
}

// Transition to FRIGHTENED state
void Ghost::setFrightenedState(const std::chrono::milliseconds duration) {
  // Dead ghosts should spawn frightened
  m_frightenedDurationMs = duration;
  if (m_ghostState != GhostState::FRIGHTENED) {
    m_logger->logInfo("Setting frightened state duration: " +
                      std::to_string(duration.count()) + "ms");
    // But reverse direction only once!
    reverseDirection();
  }
  // Set ghost state to frightened, if it spawns it will be frightened
  setGhostState(GhostState::FRIGHTENED);
}

void Ghost::setDeadState() {
  setCurrDirection(EntityDirection::NONE);
  setNextDirection(EntityDirection::NONE);
  setMovementState(MovementState::ON_TILE);
  setEntityState(EntityState::DEAD);
}

void Ghost::revertFromFrightenedState() {
  if (m_ghostState == GhostState::FRIGHTENED) {
    m_logger->logInfo("Reverting from FRIGHTENED state to: " +
                      Entities::toString(m_previousGhostState));
    m_frightenedDurationMs = 0ms;
    setGhostState(m_previousGhostState);
  }
}

void Ghost::reverseDirection() {
  EntityDirection reversed = EntityDirection::NONE;
  switch (getCurrDirection()) {
  case EntityDirection::UP:
    reversed = EntityDirection::DOWN;
    break;
  case EntityDirection::DOWN:
    reversed = EntityDirection::UP;
    break;
  case EntityDirection::LEFT:
    reversed = EntityDirection::RIGHT;
    break;
  case EntityDirection::RIGHT:
    reversed = EntityDirection::LEFT;
    break;
  case EntityDirection::NONE:
    break;
  }
  if (reversed != EntityDirection::NONE) {
    m_logger->logDebug("Reversing direction from " +
                       Entities::toString(getCurrDirection()) + " to " +
                       Entities::toString(reversed));
    setCurrDirection(reversed);
    setNextDirection(EntityDirection::NONE);
  }
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan