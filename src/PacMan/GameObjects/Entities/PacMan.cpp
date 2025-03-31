//
// Created by Jakub Szwedowicz on 2/23/25.
//

#include "Entities/PacMan.h"
#include "GameObjects/Level.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

using namespace std::chrono_literals;

PacMan::PacMan(TilePosition startingPosition, Level& level, GameEvents::GameEventsManager &gameEventsPublisher)
    : MovingEntity(EntityType::PAC_MAN, startingPosition, level),
      ISubscriber(&gameEventsPublisher.getEntityEventPublisher()),
      m_logger(
          std::make_unique<Utils::Logger>("PacMan", Utils::LogLevel::INFO)),
      m_entityEventsPublisher(gameEventsPublisher.getEntityEventPublisher()) {}

void PacMan::update(std::chrono::milliseconds deltaTime) {
  if (m_pacManState == PacManState::EMPOWERED) {
    m_empoweredDurationMs -= deltaTime;
    if (m_empoweredDurationMs <= 0ms) {
      m_entityEventsPublisher.publish(GameEvents::PowerPelletExpiredEvent(m_entityId));
    }
  }
}

void PacMan::callback(const GameEvents::EntityEvent &event) {
  if (event.entityId != m_entityId) {
    return;
  }
  // Handle events targeted at this PacMan instance
  switch (event.eventType) {
  case GameEvents::EntityEventType::PLAYER_DIED_EVENT: {
    const auto &caughtEvent =
        static_cast<const GameEvents::PlayerDiedEvent &>(event);

    m_logger->logInfo(
        "PacMan died by Ghost ID: " + std::to_string(caughtEvent.ghostId) +
        " at (" + std::to_string(caughtEvent.position.x) + "," +
        std::to_string(caughtEvent.position.y) + ")");

    // 2. Stop movement
    setCurrDirection(EntityDirection::NONE);
    setNextDirection(EntityDirection::NONE);
    setMovementState(MovementState::ON_TILE);

    // 3. Trigger death sequence/animation (this might involve publishing
    setEntityState(EntityState::DEAD);
    decreaseHealth();

    // Reset PacMan state if it was empowered
    setNormalPacManState();
    break;
  }
  case GameEvents::EntityEventType::PLAYER_RESPAWNED_EVENT: {
    const auto &respawnEvent =
        static_cast<const GameEvents::PlayerRespawnedEvent &>(event);

    m_logger->logInfo("PacMan respawned at (" +
                      std::to_string(respawnEvent.spawnPosition.x) + "," +
                      std::to_string(respawnEvent.spawnPosition.y) + ")");

    // Reset position
    setTilePosition(respawnEvent.spawnPosition);

    // Reset state
    setEntityState(EntityState::ALIVE); // Assuming EntityState exists
    setMovementState(MovementState::ON_TILE);
    setNormalPacManState();

    // Reset direction
    setCurrDirection(EntityDirection::NONE);
    // Do not reset this one. Allow to execute a player movement immediately if
    // it was set setNextDirection(EntityDirection::NONE);
    break;
  }
  case GameEvents::EntityEventType::PELLET_EATEN_EVENT: {
    const auto &pelletEvent =
        static_cast<const GameEvents::PelletEatenEvent &>(event);
    // Log confirmation.
    m_logger->logDebug(
        "Processing PELLET_EATEN event at (" +
        std::to_string(pelletEvent.position.x) + "," +
        std::to_string(pelletEvent.position.y) +
        ") ScoreValue: " + std::to_string(pelletEvent.scoreValue));
    increaseScore(pelletEvent.scoreValue);
    break;
  }
  case GameEvents::EntityEventType::POWER_PELLET_EATEN_EVENT: {
    const auto &powerPelletEvent =
        dynamic_cast<const GameEvents::PowerPelletEatenEvent &>(event);
    m_logger->logInfo("PacMan ate power pellet eaten at (" +
                      std::to_string(powerPelletEvent.position.x) + "," +
                      std::to_string(powerPelletEvent.position.y) + ")");
    increaseScore(powerPelletEvent.scoreValue);
    setEmpoweredPacManState(powerPelletEvent.frightenDuration);
    break;
  }
  case GameEvents::EntityEventType::POWER_PELLET_EXPIRED_EVENT: {
    [[maybe_unused]] const auto &expiredEvent =
        static_cast<const GameEvents::PowerPelletExpiredEvent &>(event);

    if (m_pacManState == PacManState::EMPOWERED) {
      m_logger->logInfo("PacMan received Power Pellet expired event.");
      setNormalPacManState();
    } else {
      m_logger->logWarning(
          "Received Power Pellet expired event, but PacMan was not empowered.");
    }
    break;
  }
  case GameEvents::EntityEventType::ENTITY_MOVED_EVENT: {
    const auto &movedEvent =
        static_cast<const GameEvents::EntityMovedEvent &>(event);
    m_logger->logDebug("Processing ENTITY_MOVED event: Moved from (" +
                       std::to_string(movedEvent.previousPosition.x) + "," +
                       std::to_string(movedEvent.previousPosition.y) +
                       ") to (" + std::to_string(movedEvent.newPosition.x) +
                       "," + std::to_string(movedEvent.newPosition.y) + ")");
    setTilePosition(movedEvent.newPosition);
    break;
  }

  case GameEvents::EntityEventType::GHOST_EATEN_EVENT: {
    // Event confirms PacMan ate a frightened ghost.
    const auto &ghostEatenEvent =
        static_cast<const GameEvents::GhostEatenEvent &>(event);
    // Log confirmation, including which ghost was eaten.
    m_logger->logInfo(
        "Processing GHOST_EATEN event: Ate Ghost ID " +
        std::to_string(ghostEatenEvent.eatenGhostId) + " at (" +
        std::to_string(ghostEatenEvent.position.x) + "," +
        std::to_string(ghostEatenEvent.position.y) +
        ") ScoreValue: " + std::to_string(ghostEatenEvent.scoreValue));
    increaseScore(ghostEatenEvent.scoreValue);
    break;
  }

    // --- Ignored Cases ---
  case GameEvents::EntityEventType::GHOST_STATE_CHANGED_EVENT:
    break;
  default:
    m_logger->logError("Received unhandled EntityEvent type: " +
                       GameEvents::toString(event.eventType));
    break;
  }
}

void PacMan::setPacManState(const PacManState pacManState) {
  m_pacManState = pacManState;
}

void PacMan::setNormalPacManState() {
  m_empoweredDurationMs = 0ms;
  setPacManState(PacManState::NORMAL);
}

void PacMan::setEmpoweredPacManState(
    const std::chrono::milliseconds empoweredDuration) {
  m_empoweredDurationMs = empoweredDuration;
  setPacManState(PacManState::EMPOWERED);
}

std::unique_ptr<PacMan> PacManBuilder::build() {
  return std::make_unique<PacMan>(startingPosition, *level, gameEventsManager);
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan