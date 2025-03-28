//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include "GameLogic/GameRunner.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameLogic {

GameRunner::GameRunner(int gameId, std::unique_ptr<Level> level)
    : m_gameId(gameId), m_level(std::move(level)) {
  m_logger.logInfo("Created GameRunner with id '" + std::to_string(m_gameId) +
                   "'");
}

bool GameRunner::startGame() {
  if (m_gameStatus != GameStatus::WAITING) {
    m_logger.logError("Game '" + std::to_string(m_gameId) +
                      "' has already been started!");
    return false;
  }

  if (m_level == nullptr || !m_level->isReady()) {
    m_logger.logError("Game '" + std::to_string(m_gameId) + "' has no level!");
    return false;
  }

  m_logger.logInfo("Starting Game '" + std::to_string(m_gameId) + "'");

  m_gameStatus = GameStatus::RUNNING;
  m_gameThread = std::thread(&GameRunner::gameLoop, this);

  return true;
}

void GameRunner::gameLoop() {
  auto lastUpdateTime = std::chrono::high_resolution_clock::now();
  const std::chrono::milliseconds targetUpdateTime(100);

  while (m_gameStatus == GameStatus::RUNNING) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastUpdateTime);

    if (elapsedTime >= targetUpdateTime) {
      update(elapsedTime);
      lastUpdateTime = currentTime;
    } else {
      std::this_thread::sleep_for(targetUpdateTime - elapsedTime);
    }
  }
  m_logger.logInfo("Game '" + std::to_string(m_gameId) +
                   " changed status to '" + toString(m_gameStatus) + "'");
}

void GameRunner::update(std::chrono::milliseconds deltaTime) {
  if (!m_level || m_level->getBoard().empty())
    return;

  // Move players in their directions
  for (auto const &pacman : m_level->getPacmans()) {
    updateMovingEntity(*pacman, deltaTime);
  }

  // Move Ghosts in their directions
  for (auto const &ghost : m_level->getGhosts()) {
    updateMovingEntity(*ghost, deltaTime);
  }

  // Handle collisions
  // handleCollisions(board);

  // Check for game over conditions
  if (checkGameOver()) {
    m_gameStatus = GameStatus::FINISHED;
    m_logger.logInfo("Game '" + std::to_string(m_gameId) + "' finished!");
  }

  // TODO: trigger an event to notify clients about the updated game state
}

void GameRunner::updateMovingEntity(MovingEntity &movingEntity,
                                    std::chrono::milliseconds deltaTime) {
  // Update direction of the movement when ON_TILE
  if (movingEntity.getMovementState() == MovementState::ON_TILE &&
      movingEntity.getNextDirection() != EntityDirection::NONE) {
    if (canMove(movingEntity.getTilePosition(),
                movingEntity.getNextDirection())) {
      movingEntity.setCurrDirection(movingEntity.getNextDirection());
      movingEntity.setNextDirection(EntityDirection::NONE);
      movingEntity.setMovementState(MovementState::IN_TRANSIT);
    }
  }

  if (movingEntity.getMovementState() == MovementState::IN_TRANSIT) {

    // We are in transit. Check if we reach the next tile.
    float distance = movingEntity.getSpeedIn100Ms() * deltaTime.count();
    auto newRealPosition =
        moveTowards(movingEntity.getRealPosition(),
                    movingEntity.getCurrDirection(), distance);
    movingEntity.setRealPosition(newRealPosition);

    const auto &currentTilePosition = movingEntity.getTilePosition();
    auto newTilePosition = TilePosition(newRealPosition);
    // Check if we are still in transit between the tiles.
    if (newTilePosition.x == currentTilePosition.x &&
        newTilePosition.y == currentTilePosition.y) {
      return;
    }

    // TODO: Handle collisions!
    // auto &entityOnTile =
    //     m_level->getBoard()
    //     // Handle pellet consumption
    //     if (newGridX >= 0 && newGridX < board[0].size() && newGridY >= 0 &&
    //         newGridY < board.size() && board[newGridY][newGridX]) {
    //   if (board[newGridY][newGridX]->getSymbol() ==
    //       '.') {                           // Assuming '.' represents a
    //       pellet
    //     board[newGridY][newGridX].reset(); // Remove the pellet from the
    //     board m_score += 10; std::cout << "Player ate a pellet! Score: " <<
    //     m_score << std::endl;
    //     // Check if all pellets are eaten (you'd need a way to track this in
    //     // the Level class)
    //   } else if (board[newGridY][newGridX]->getSymbol() ==
    //              'o') {
    //     // Assuming 'o' represents a power pellet
    //     board[newGridY][newGridX].reset(); // Remove the power pellet
    //     m_score += 50;
    //     std::cout << "Player ate a power pellet!" << std::endl;
    //     // Set all ghosts to vulnerable state
    //     for (const auto &row : board) {
    //       for (const auto &entityPtr : row) {
    //         if (auto ghost =
    //                 dynamic_cast<PacMan::Entities::Ghost *>(entityPtr.get()))
    //                 {
    //           ghost->setState(PacMan::Entities::Ghost::State::VULNERABLE);
    //           ghost->setVulnerableTimer(
    //               std::chrono::seconds(10)); // Example duration
    //                 }
    //       }
    //     }
    //   }
  }
  return;
}

bool GameRunner::canMove(const TilePosition &tilePosition,
                         const EntityDirection &direction) const {
  auto nextPosition = moveTowardsGrid(tilePosition, direction, 1);

  if (m_level->isTilePositionValid(nextPosition)) {
    return m_level->getEntityOnTile(nextPosition)->getEntityType() !=
           EntityType::WALL;
  }
  return false;
}

RealPosition GameRunner::moveTowards(const RealPosition &realPosition,
                                     const EntityDirection &direction,
                                     const float distance) const {
  RealPosition newPosition = realPosition;
  // TODO: get back here if distance is sometimes too big!
  if (distance >= 2.0f) {
    m_logger.logError("Distance to move entity in '" +
                      std::to_string(distance) + "' is greater than 2.0!");
  }
  if (distance >= 1.0f)
    switch (direction) {
    case GameObjects::Entities::EntityDirection::UP:
      newPosition.y -= distance;
      break;
    case GameObjects::Entities::EntityDirection::DOWN:
      newPosition.y += distance;
      break;
    case GameObjects::Entities::EntityDirection::LEFT:
      newPosition.x -= distance;
      break;
    case GameObjects::Entities::EntityDirection::RIGHT:
      newPosition.x += distance;
      break;
    case GameObjects::Entities::EntityDirection::NONE:
      m_logger.logError("Tested tile position " + realPosition.toString() +
                        " for direction " + toString(direction) +
                        " and distance " + std::to_string(distance) + "!");
      break;
    }

  return realPosition;
}

TilePosition GameRunner::moveTowardsGrid(const TilePosition &tilePosition,
                                         const EntityDirection &direction,
                                         const int distance) const {
  auto newPosition = RealPosition(tilePosition);
  newPosition =
      moveTowards(newPosition, direction, static_cast<float>(distance));
  return TilePosition(newPosition);
}

bool GameRunner::checkGameOver() const {
  // Example: Game over if no players are left or no pellets remaining
  if (std::all_of(m_level->getPacmans().cbegin(), m_level->getPacmans().cend(),
                  [](const PacMan *pacman) -> bool {
                    return pacman->getEntityState() == EntityState::DEAD;
                  })) {
    m_logger.logInfo("Game Over!, all players are dead.");
    return true;
  }

  // Check for remaining pellets
  if (m_level->getNumberOfFood() == 0) {
    m_logger.logInfo("Game Over!, all the food is gone.");
    return true;
  }

  return false;
}

} // namespace GameLogic
} // namespace PacMan
