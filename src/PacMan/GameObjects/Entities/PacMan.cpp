//
// Created by Jakub Szwedowicz on 2/23/25.
//

#include "Entities/PacMan.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

using namespace std::chrono_literals;

PacMan::PacMan(Level *level)
    : MovingEntity(EntityType::PAC_MAN, level),
      m_logger(
          std::make_unique<Utils::Logger>("PacMan", Utils::LogLevel::INFO)) {}

void PacMan::update(std::chrono::milliseconds deltaTime) {
  if (m_pacManState == PacManState::EMPOWERED) {
    m_empoweredTime -= deltaTime;
    if (m_empoweredTime <= 0ms) {
      m_gameEventsManager.publish(GameEvents::PowerPelletExpired(m_entityId));
    }
  }
}

void PacMan::callback(const GameEvents::EntityEvent &event) {
  if (event.entityId != m_entityId) {
    return;
  }
  // Handle events targeted at this PacMan instance
  switch (event.eventType) {
  case GameEvents::EntityEventType::PLAYER_DIED: {
    const auto &caughtEvent =
        static_cast<const GameEvents::PlayerDied &>(event);

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
    setPacmanState(PacManState::NORMAL);
    break;
  }
  case GameEvents::EntityEventType::PLAYER_RESPAWNED: {
    const auto &respawnEvent =
        static_cast<const GameEvents::PlayerRespawned &>(event);

    m_logger->logInfo("PacMan respawned at (" +
                      std::to_string(respawnEvent.spawnPosition.x) + "," +
                      std::to_string(respawnEvent.spawnPosition.y) + ")");

    // Reset position
    setTilePosition(respawnEvent.spawnPosition);

    // Reset state
    setEntityState(EntityState::ALIVE); // Assuming EntityState exists
    setMovementState(MovementState::ON_TILE);
    setPacmanState(PacManState::NORMAL);

    // Reset direction
    setCurrDirection(EntityDirection::NONE);
    setNextDirection(EntityDirection::NONE);
    break;
  }
  case GameEvents::EntityEventType::PELLET_EATEN: {
    const auto &pelletEvent =
        static_cast<const GameEvents::PelletEaten &>(event);
    // Log confirmation.
    m_logger->logDebug(
        "Processing PELLET_EATEN event at (" +
        std::to_string(pelletEvent.position.x) + "," +
        std::to_string(pelletEvent.position.y) +
        ") ScoreValue: " + std::to_string(pelletEvent.scoreValue));
    increaseScore(pelletEvent.scoreValue);
    break;
  }
  case GameEvents::EntityEventType::POWER_PELLET_EATEN: {
    const auto &powerPelletEvent =
        dynamic_cast<const GameEvents::PowerPelletEaten &>(event);
    m_logger->logInfo("PacMan ate power pellet eaten at (" +
                      std::to_string(powerPelletEvent.position.x) + "," +
                      std::to_string(powerPelletEvent.position.y) + ")");
    increaseScore(powerPelletEvent.scoreValue);
    setPacmanState(PacManState::EMPOWERED);
    m_empoweredTime = s_empoweredTime;
    break;
  }
  case GameEvents::EntityEventType::POWER_PELLET_EXPIRED: {
    [[maybe_unused]] const auto &expiredEvent =
        static_cast<const GameEvents::PowerPelletExpired &>(event);

    if (m_pacManState == PacManState::EMPOWERED) {
      m_logger->logInfo("PacMan received Power Pellet expired event.");
      setPacmanState(PacManState::NORMAL);
      m_empoweredTime = 0ms;
    } else {
      m_logger->logWarning(
          "Received Power Pellet expired event, but PacMan was not empowered.");
    }
    break;
  }
  case GameEvents::EntityEventType::ENTITY_MOVED: {
    const auto &movedEvent =
        static_cast<const GameEvents::EntityMoved &>(event);
    m_logger->logDebug("Processing ENTITY_MOVED event: Moved from (" +
                       std::to_string(movedEvent.previousPosition.x) + "," +
                       std::to_string(movedEvent.previousPosition.y) +
                       ") to (" + std::to_string(movedEvent.newPosition.x) +
                       "," + std::to_string(movedEvent.newPosition.y) + ")");
    setTilePosition(movedEvent.newPosition);
    break;
  }

  case GameEvents::EntityEventType::GHOST_EATEN: {
    // Event confirms PacMan ate a frightened ghost.
    const auto &ghostEatenEvent =
        static_cast<const GameEvents::GhostEaten &>(event);
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
  case GameEvents::EntityEventType::GHOST_STATE_CHANGED:
    break;
  default:
    m_logger->logError("Received unhandled EntityEvent type: " +
                       GameEvents::toString(event.eventType));
    break;
  }
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan