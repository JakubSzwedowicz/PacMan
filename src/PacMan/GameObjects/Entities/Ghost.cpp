//
// Created by Jakub Szwedowicz on 2/24/25.
//

#include <limits>
#include <chrono>

#include "Entities/Ghost.h"

#include "Entities/PacMan.h"
#include "GameObjects/Level.h"
#include "PathFinders/AStarPathFinder.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

using namespace std::chrono_literals;
Ghost::Ghost(GhostType ghostType, TilePosition startingPosition, Level &level,
             GameEvents::GameEventsManager &gameEventsManager)
    : MovingEntity(EntityType::GHOST, startingPosition, level),
      ISubscriber(&gameEventsManager.getEntityEventPublisher()),
      m_logger(
          std::make_unique<Utils::Logger>(toString(), Utils::LogLevel::INFO)),
      m_ghostType(ghostType),
      m_scatterPosition(m_level.getScatteringPositionOfGhost(m_ghostType)),
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
  case GameEvents::EntityEventType::POWER_PELLET_EATEN_EVENT: {
    const auto &pelletEvent =
        static_cast<const GameEvents::PowerPelletEatenEvent &>(event);
    m_logger->logDebug("Processing POWER_PELLET_EATEN event.");
    setFrightenedState(pelletEvent.frightenDuration);
    break;
  }

  case GameEvents::EntityEventType::POWER_PELLET_EXPIRED_EVENT: {
    m_logger->logDebug("Processing POWER_PELLET_EXPIRED event.");
    // ghosts clears frightened state in update(...)
    break;
  }

  case GameEvents::EntityEventType::GHOST_EATEN_EVENT: {
    const auto &eatenEvent =
        static_cast<const GameEvents::GhostEatenEvent &>(event);
    if (eatenEvent.eatenGhostId == m_entityId) {
      m_logger->logDebug("Processing GHOST_EATEN event: " +
                         eatenEvent.toString());
      setDeadState();
    }
    break;
  }

    // --- Events Ghosts Might Log or Ignore ---
  case GameEvents::EntityEventType::ENTITY_MOVED_EVENT: {
    if (event.entityId == m_entityId) {
      const auto &movedEvent =
          static_cast<const GameEvents::EntityMovedEvent &>(event);
      m_logger->logDebug(
          "Processing own ENTITY_MOVED event: " + movedEvent.toString() +
          " To (" + std::to_string(movedEvent.newPosition.x) + "," +
          std::to_string(movedEvent.newPosition.y) + ")");
      // We do nothing here. Decisions are made in junctions!
    }
    break;
  }

  case GameEvents::EntityEventType::ENTITY_AT_JUNCTION_EVENT: {
    if (event.entityId == m_entityId) {
      const auto &entityAtJunctionEvent =
          static_cast<const GameEvents::EntityAtJunctionEvent &>(event);
      m_logger->logDebug(
          "Processing own ENTITY_AT_JUNCTION event: " +
          entityAtJunctionEvent.toString() + " To (" +
          std::to_string(entityAtJunctionEvent.junctionPosition.x) + "," +
          std::to_string(entityAtJunctionEvent.junctionPosition.y) + ")");
      EntityDirection nextDirection =
          getDirectionAtJunction(entityAtJunctionEvent.junctionPosition);
      setNextDirection(nextDirection);
    }
    break;
  }

  case GameEvents::EntityEventType::GHOST_STATE_CHANGED_EVENT: {
    // Log own state change confirmation if needed
    if (event.entityId == m_entityId) {
      const auto &stateEvent =
          static_cast<const GameEvents::GhostStateChangedEvent &>(event);
      m_logger->logDebug("Received confirmation of own state change to: " +
                         Entities::toString(stateEvent.newState));
    } else {
      // TODO: Do nothing here unless I decide on implementing some smarter AI
    }
    break;
  }

    // --- Ignored Events ---
  case GameEvents::EntityEventType::SCORED_UPDATED_EVENT:
    break;
  case GameEvents::EntityEventType::PELLET_EATEN_EVENT:
    break;
  case GameEvents::EntityEventType::PLAYER_DIED_EVENT:
    break;
  case GameEvents::EntityEventType::PLAYER_RESPAWNED_EVENT:
    break;
  default:
    m_logger->logError("Received unhandled EntityEvent: " + event.toString());
    break;
  }
}

TilePosition Ghost::getScatteringPosition() const { return m_scatterPosition; }
void Ghost::setScatteringPosition(TilePosition scatteringPosition) {
  m_scatterPosition = scatteringPosition;
}

void Ghost::setGhostStrategies(
    const GhostStateToGhostStrategies_t &ghostStrategies) {
  for (const auto &[state, strategy] : ghostStrategies) {
    m_ghostStrategies[state] = strategy->clone();
  }
}

std::string Ghost::toString() const {
  return "Ghost(" + std::to_string(m_entityId) + ", " +
         Entities::toString(m_ghostType) + ")";
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
  m_entityEventsPublisher.publish(GameEvents::GhostStateChangedEvent(
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

EntityDirection
Ghost::getDirectionBasedOnAdjacentTiles(TilePosition start,
                                        TilePosition adjacent) const {
  TilePosition vec = adjacent - start;
  if (vec.x == 0) {
    // Positive 'y' means 'adjacent' is lower than 'start'.
    return vec.y > 0 ? EntityDirection::DOWN : EntityDirection::UP;
  } else if (vec.y == 0) {
    // Positive 'x' means 'adjacent' is to the right of the 'start'
    return vec.x > 0 ? EntityDirection::RIGHT : EntityDirection::LEFT;
  }
  m_logger->logWarning("Looks suspicious! Returning direction " +
                       Entities::toString(EntityDirection::NONE) +
                       " for the entity: " + toString() + " when going from " +
                       start.toString() + " to " + adjacent.toString());
  return EntityDirection::NONE;
}

EntityDirection
Ghost::getDirectionAtJunction(TilePosition junctionPosition) const {
  PacMan *targetPacMan = nullptr;
  {
    float minValue = std::numeric_limits<float>::max();
    for (const auto &pacman : m_level.getPacMans()) {
      float computedValue =
          GameLogic::Strategies::PathFinders::Heuristics::manhattanDistance(
              junctionPosition, pacman->getTilePosition());
      if (computedValue < minValue) {
        minValue = computedValue;
        targetPacMan = pacman.get();
      }
    }
  }
  Ghost *blinkyOrAny = m_level.getGhostOrAnyButNot(*this, GhostType::BLINKY);
  TilePosition maybeBlinkyPosition =
      (blinkyOrAny ? blinkyOrAny->getTilePosition()
                   : (targetPacMan ? targetPacMan->getTilePosition()
                                   : getStartingPosition()));
  GameLogic::Strategies::StrategyContext context = {
      *this, (*targetPacMan), m_level, maybeBlinkyPosition};
  TilePosition target =
      m_ghostStrategies.at(m_ghostState)->getTargetTile(context);
  // TODO: Consider caching this path. Will revaluate based on framerate and
  // lag.
  auto path = GameLogic::Strategies::PathFinders::AStarPathFinder::findPath(
      getTilePosition(), target, m_level);

  return getDirectionBasedOnAdjacentTiles(getTilePosition(), path.front());
}

std::unique_ptr<Ghost> GhostBuilder::build() {
  return std::make_unique<Ghost>(ghostType, startingPosition, *level,
                                 gameEventsManager);
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan