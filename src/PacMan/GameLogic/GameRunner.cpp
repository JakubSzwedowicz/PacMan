//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include "GameLogic/GameRunner.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameLogic {

GameRunner::GameRunner(int gameId, std::unique_ptr<GameObjects::Level> level)
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

  auto &board = m_level->getBoard();
  size_t height = board.size();
  size_t width = board[0].size();

  // Move players in their directions
  for (auto const &pacman : m_level->getPacmans()) {
    updatePacMan(*pacman, deltaTime, board);
  }

  // Move Ghosts in their directions
  for (auto const &ghost : m_level->getGhosts()) {
    updateGhost(*ghost, deltaTime, board);
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

void GameRunner::updatePacMan(PacMan::GameObjects::Entities::PacMan &pacman,
                              std::chrono::milliseconds deltaTime,
                              PacMan::GameObjects::Level::Board_t &board) {
  // Move in the current direction
  if (pacman.getCurrDirection() !=
      GameObjects::Entities::EntityDirection::NONE) {
    float distance = pacman.getSpeedIn100Ms() * deltaTime.count();
    GameObjects::Entities::RealPosition realPosition = pacman.getRealPosition();

    moveTowardsGrid(newGridX, newGridY, pacman.getCurrDirection(), distance);

    // If pacman arrives to the neighbouring cell
    if (newGridX >= 0 && newGridX < m_level->getWidth() && newGridY >= 0 &&
        newGridY < m_level->getHeight() &&
        board[newGridY][newGridX].get() != &pacman) {

      pacman.setX(static_cast<float>(newGridX));
      pacman.setY(static_cast<float>(newGridY));

      // Handle pellet consumption
      if (newGridX >= 0 && newGridX < board[0].size() && newGridY >= 0 &&
          newGridY < board.size() && board[newGridY][newGridX]) {
        if (board[newGridY][newGridX]->getSymbol() ==
            '.') {                           // Assuming '.' represents a pellet
          board[newGridY][newGridX].reset(); // Remove the pellet from the board
          m_score += 10;
          std::cout << "Player ate a pellet! Score: " << m_score << std::endl;
          // Check if all pellets are eaten (you'd need a way to track this in
          // the Level class)
        } else if (board[newGridY][newGridX]->getSymbol() ==
                   'o') { // Assuming 'o' represents a power pellet
          board[newGridY][newGridX].reset(); // Remove the power pellet
          m_score += 50;
          std::cout << "Player ate a power pellet!" << std::endl;
          // Set all ghosts to vulnerable state
          for (const auto &row : board) {
            for (const auto &entityPtr : row) {
              if (auto ghost = dynamic_cast<PacMan::Entities::Ghost *>(
                      entityPtr.get())) {
                ghost->setState(PacMan::Entities::Ghost::State::VULNERABLE);
                ghost->setVulnerableTimer(
                    std::chrono::seconds(10)); // Example duration
              }
            }
          }
        }
      }
    }
  }

  // Attempt to move in the next direction
  if (pacman.getNextDirection() !=
      GameObjects::Entities::EntityDirection::NONE) {
    if (canMove(pacman.getTilePosition(), pacman.getNextDirection(), board)) {
      pacman.setCurrDirection(pacman.getNextDirection());
      pacman.setNextDirection(GameObjects::Entities::EntityDirection::NONE);
    }
  }
}

void GameRunner::updateGhost(GameObjects::Entities::Ghost &ghost,
                             std::chrono::milliseconds deltaTime,
                             PacMan::GameObjects::Level::Board_t &board) {
  // ghost.update(deltaTime, *m_level, x, y); // Pass level and PacMan positions
  // for AI
}

bool GameRunner::canMove(GameObjects::Entities::TilePosition tilePosition,
                         GameObjects::Entities::EntityDirection direction,
                         PacMan::GameObjects::Level::Board_t &board) const {
  auto nextPosition = moveTowards(
      GameObjects::Entities::RealPosition(tilePosition), direction, 1.0f);

  auto tilePosition = GameObjects::Entities::TilePosition(nextPosition);
  if (tilePosition.x >= 0 && tilePosition.x < m_level->getWidth() &&
      tilePosition.y >= 0 && tilePosition.y < m_level->getHeight()) {
    return m_level->getBoard()[tilePosition.y][tilePosition.x]
               ->getEntityType() != GameObjects::Entities::EntityType::WALL;
  }
  return false;
}

GameObjects::Entities::RealPosition
GameRunner::moveTowards(GameObjects::Entities::RealPosition realPosition,
                        GameObjects::Entities::EntityDirection direction,
                        float distance) const {
  GameObjects::Entities::RealPosition newPosition = realPosition;
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
    break;
  }
  return realPosition;
}

bool GameRunner::moveTowardsGrid(
    GameObjects::Entities::RealPosition &realPosition,
    GameObjects::Entities::EntityDirection direction, float distance) const {
  // For grid-based movement, we're essentially trying to move to the next cell
  // The distance here might be used to determine if we've reached the next
  // cell. For simplicity, let's just move one grid cell at a time based on
  // direction
  // TODO: get back here if distance is sometimes too big!
  if (distance >= 2.0f)
    m_logger.logError("Distance to move entity in '" +
                      std::to_string(distance) + "' is greater than 2.0!");
  if (distance >= 1.0f) {
    switch (direction) {
    case GameObjects::Entities::EntityDirection::UP:
      y--;
      break;
    case GameObjects::Entities::EntityDirection::DOWN:
      y++;
      break;
    case GameObjects::Entities::EntityDirection::LEFT:
      x--;
      break;
    case GameObjects::Entities::EntityDirection::RIGHT:
      x++;
      break;
    case GameObjects::Entities::EntityDirection::NONE:
      break;
    }
  }
}

bool GameRunner::checkGameOver() const {
  // Example: Game over if no players are left or no pellets remaining
  if (std::all_of(
          m_level->getPacmans().cbegin(), m_level->getPacmans().cend(),
          [](const PacMan::GameObjects::Entities::PacMan *pacman) -> bool {
            return pacman->getEntityState() ==
                   GameObjects::Entities::EntityState::DEAD;
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

}

} // namespace GameLogic
} // namespace PacMan