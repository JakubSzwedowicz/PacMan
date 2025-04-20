//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <vector>

#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameLogic/GameRunner.h"
#include "GameObjects/Level.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameLogic {

GameRunner::GameRunner(int gameRunnerId, std::unique_ptr<Level> level,
                       GameEvents::GameEventsManager &gameEventsManager)
    : ISubscriber(&gameEventsManager.getGameEventPublisher()),
      m_gameRunnerId(gameRunnerId), m_level(std::move(level)),
      m_logger(std::make_unique<Utils::Logger>("GameRunner",
                                               Utils::LogLevel::DEBUG)),
      m_gameEventsManager(gameEventsManager) {
  m_logger->logInfo("Created GameRunner with id '" +
                    std::to_string(m_gameRunnerId) + "'");
}

bool GameRunner::startGame() {
  if (m_gameStatus != GameStatus::CREATING) {
    m_logger->logError("Game '" + std::to_string(m_gameRunnerId) +
                       "' has already transitioned to state " +
                       toString(GameStatus::WAITING) + "!");
    return false;
  }

  if (m_level == nullptr || !m_level->isReady()) {
    m_logger->logError("Game '" + std::to_string(m_gameRunnerId) +
                       "' has no level!");
    return false;
  }

  m_logger->logInfo("Waiting for players to connect '" +
                    std::to_string(m_gameRunnerId) + "' on separate thread");
  const auto gameStatusRunning = GameStatus::WAITING;
  m_gameEventsManager.getGameEventPublisher().publish(
      GameEvents::GameStatusChanged(m_gameRunnerId, gameStatusRunning));
  m_gameThread = std::thread(&GameRunner::gameLoop, this);

  return true;
}

void GameRunner::callback(const GameEvents::GameEvent &event) {
  switch (event.gameEventType) {
  case (GameEvents::GameEventType::GAME_STATUS_CHANGED):
    const auto &gameStatusChangedEvent =
        static_cast<const GameEvents::GameStatusChanged &>(event);
    m_gameStatus = gameStatusChangedEvent.gameStatus;
    switch (gameStatusChangedEvent.gameStatus) {
    case GameStatus::CREATING:
      break;
    case GameStatus::WAITING:
      break;
    case GameStatus::RUNNING:
      break;
    case GameStatus::PAUSED:
      break;
    case GameStatus::FINISHED:
      break;
    }
    break;
  }
  m_gameLoopRunningLoopCV.notify_one();
}

void GameRunner::printToCLI() const {
  if (m_gameRunnerId != 0) {
    m_logger->logCritical("This function can print first and only game!");
    return;
  }

  static std::vector<std::string> previousBuffer;
  static std::vector<std::string> buffer;
  buffer.reserve(m_level->getHeight() + 10);

  // --- Render Header (Optional) ---
  const int headerOffset = 2;
  buffer.push_back("---- Pac-Man Game ID: " + std::to_string(m_gameRunnerId) +
                   " ----\n");
  buffer.push_back("----------------------------\n");

  // --- Print Board ---
  const size_t height = m_level->getHeight();
  const size_t width = m_level->getWidth();

  for (size_t row = 0; row < height; ++row) {
    std::string line;
    for (size_t col = 0; col < width; ++col) {
      TilePosition currentPos = {static_cast<int>(col), static_cast<int>(row)};
      // Assuming Level provides a way to get the type at a position
      EntityType type = m_level->getEntityOnTile(currentPos);
      line += Entities::toChar(type);
    }
    line += "\n";
    buffer.push_back(line);
  }

  for (const auto &movingEntities : m_level->getMovingEntities()) {
    buffer[headerOffset + movingEntities->getTilePosition().y]
          [movingEntities->getTilePosition().x] =
              Entities::toChar(movingEntities->getEntityType());
  }

  // --- Print Footer (Optional) ---
  const int footerOffset = 2;
  buffer.emplace_back("----------------------------\n");
  buffer.push_back("Status: " + toString(m_gameStatus.load()) + "\n");

  if (previousBuffer.empty()) {
    for (const auto &row : buffer)
      std::cout << row;
  } else {
    // Delta update
    for (size_t row = 0; row < buffer.size(); ++row) {
      // Ensure previous buffer has this line before comparing characters
      if (row < previousBuffer.size()) {
        size_t lineLength = buffer[row].length();
        size_t prevLineLength = previousBuffer[row].length();

        for (size_t col = 0; col < lineLength; ++col) {
          // Get current char and previous char (handle length diff)
          char current_char = buffer[row][col];
          char previous_char =
              (col < prevLineLength)
                  ? previousBuffer[row][col]
                  : '\0'; // Use null if x exceeds previous line length

          if (current_char != previous_char) {
            // Move cursor (1-based)
            std::cout << "\033[" << (row + 1) << ";" << (col + 1) << "H";
            // Print the new character
            std::cout << current_char;
          }
        }
        // If the new line is shorter than the old line, clear the rest
        if (lineLength < prevLineLength) {
          std::cout << "\033[" << (row + 1) << ";" << (lineLength + 1)
                    << "H"; // Move after last char
          for (size_t k = lineLength; k < prevLineLength; ++k) {
            std::cout << ' '; // Overwrite with spaces
          }
        }
      } else {
        // New line added compared to previous buffer - print the whole line
        std::cout << "\033[" << (row + 1) << ";1H"; // Move to start of line
        std::cout << buffer[row];                   // Print the new line
      }
    }
    // If previous buffer had more lines than current, clear them (less likely
    // here)
    if (previousBuffer.size() > buffer.size()) {
      for (size_t y = buffer.size(); y < previousBuffer.size(); ++y) {
        std::cout << "\033[" << (y + 1) << ";1H"; // Move to start of line
        std::cout << "\033[K"; // ANSI code to clear line from cursor to end
      }
    }

    // Move cursor after the printed content
    std::cout << "\033[" << (buffer.size() + 1) << ";1H";
    std::cout << std::flush; // Flush output
  }
  previousBuffer = std::move(buffer);
}

void GameRunner::gameLoop() {
  auto lastUpdateTime = std::chrono::high_resolution_clock::now();
  const std::chrono::milliseconds targetUpdateTime(100);

  constexpr int averageLoops = 10;
  unsigned long accumulateTimeInNs = 0;
  int currentLoop = 0;
  // If FINISHED close the game session.
  while (m_gameStatus != GameStatus::FINISHED) {
    std::unique_lock l(m_gameLoopRunningMutex);

    // if WAITING for players then wait for a signal to run the game
    // if for example PAUSED then wait till it's resumed
    m_gameLoopRunningLoopCV.wait(l, [this]() -> bool {
      return (m_gameStatus == GameStatus::FINISHED ||
              m_gameStatus == GameStatus::RUNNING);
    });

    // If RUNNING then run the loop iteration.
    while (m_gameStatus == GameStatus::RUNNING) {
      const auto currentTime = std::chrono::high_resolution_clock::now();
      const auto elapsedTime =
          std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                                lastUpdateTime);

      if (elapsedTime >= targetUpdateTime) {
        const auto updateBegin = std::chrono::high_resolution_clock::now();
        update(elapsedTime);
        const auto updateEnd = std::chrono::high_resolution_clock::now();
        accumulateTimeInNs +=
            std::chrono::duration_cast<std::chrono::nanoseconds>(updateEnd -
                                                                 updateBegin)
                .count();
        currentLoop++;
        if (currentLoop == averageLoops) {
          const auto averageTimePerLoopInNs = accumulateTimeInNs / currentLoop;
          m_logger->logDebug("Average loop time: " +
                             std::to_string(averageTimePerLoopInNs) + "ns");
          currentLoop = 0;
          accumulateTimeInNs = 0;
        }
        lastUpdateTime = currentTime;
      } else {
        std::this_thread::sleep_for(targetUpdateTime - elapsedTime);
      }
    }
  }
  m_logger->logInfo("Game '" + std::to_string(m_gameRunnerId) +
                    " changed status to '" + toString(m_gameStatus) + "'");
}

void GameRunner::update(std::chrono::milliseconds deltaTime) {
  // Move movingEntities in their directions
  for (auto const &movingEntities : m_level->getMovingEntities()) {
    movingEntities->update(deltaTime);
    updateMovingEntity(*movingEntities, deltaTime);
  }

  // Handle collisions
  handleCollisions();

  // Check for game over conditions
  if (checkGameOver()) {
    m_gameStatus = GameStatus::FINISHED;
    m_logger->logInfo("Game '" + std::to_string(m_gameRunnerId) +
                      "' finished!");
  }

  // TODO: trigger an event to notify clients about the updated game state
}

void GameRunner::updateMovingEntity(MovingEntity &movingEntity,
                                    std::chrono::milliseconds deltaTime) {
  // Update direction of the movement when ON_TILE
  EntityDirection direction = movingEntity.getNextDirection();
  if (movingEntity.getMovementState() == MovementState::ON_TILE &&
      direction != EntityDirection::NONE) {
    if (canMove(movingEntity.getTilePosition(), direction)) {
      movingEntity.changeDirection(direction);
    }
  }

  if (movingEntity.getMovementState() == MovementState::IN_TRANSIT) {
    // Entity is in transit. Check if it reached the next tile.
    float distance =
        movingEntity.getSpeedPerSeconds() *
        std::chrono::duration_cast<std::chrono::seconds>(deltaTime).count();
    auto newRealPosition =
        moveTowards(movingEntity.getRealPosition(),
                    movingEntity.getCurrDirection(), distance);
    movingEntity.setRealPosition(newRealPosition);

    const auto &currentTilePosition = movingEntity.getTilePosition();
    auto newTilePosition = TilePosition(newRealPosition);
    // Check if Entity is still in transit between the tiles.
    if (newTilePosition.x == currentTilePosition.x &&
        newTilePosition.y == currentTilePosition.y) {
      return;
    }
    // Entity arrived to the next tile!
    m_gameEventsManager.getEntityEventPublisher().publish(
        GameEvents::EntityMovedEvent(movingEntity.getEntityId(),
                                     currentTilePosition, newTilePosition));

    if (m_level->getEntityOnTile(newTilePosition) == EntityType::BRIDGE) {
      m_logger->logError("In game '" + std::to_string(m_gameRunnerId) +
                         " entity " + movingEntity.toString() +
                         " hit bridge which should not be possible! They are "
                         "not implemented!");
      throw std::runtime_error(
          "In game '" + std::to_string(m_gameRunnerId) + " entity " +
          movingEntity.toString() +
          " hit bridge which should not be possible! They are "
          "not implemented!");
    }
  }
}

void GameRunner::handleCollisions() {
  // Handle pellet consumption
  // if (newGridX >= 0 && newGridX < board[0].size() && newGridY >= 0 &&
  //     newGridY < board.size() && board[newGridY][newGridX]) {
  //   if (board[newGridY][newGridX]->getSymbol() ==
  //       '.') {                                  // Assuming '.' represents a
  //     pellet board[newGridY][newGridX].reset(); // Remove the pellet from the
  //     board m_score += 10;
  //     std::cout << "Player ate a pellet! Score: " << m_score << std::endl;
  //     // Check if all pellets are eaten (you'd need a way to track this in
  //     // the Level class)
  //   } else if (board[newGridY][newGridX]->getSymbol() == 'o') {
  //     // Assuming 'o' represents a power pellet
  //     board[newGridY][newGridX].reset(); // Remove the power pellet
  //     m_score += 50;
  //     std::cout << "Player ate a power pellet!" << std::endl;
  //     // Set all ghosts to vulnerable state
  //     for (const auto &row : board) {
  //       for (const auto &entityPtr : row) {
  //         if (auto ghost =
  //                 dynamic_cast<PacMan::Entities::Ghost *>(entityPtr.get())) {
  //           ghost->setState(PacMan::Entities::Ghost::State::VULNERABLE);
  //           ghost->setVulnerableTimer(
  //               std::chrono::seconds(10)); // Example duration
  //         }
  //       }
  //     }
  //   }
  // }
}

bool GameRunner::canMove(const TilePosition &tilePosition,
                         const EntityDirection &direction) const {
  auto nextPosition = moveTowardsGrid(tilePosition, direction, 1);

  return m_level->isTilePositionValid(nextPosition);
}

RealPosition GameRunner::moveTowards(const RealPosition &realPosition,
                                     const EntityDirection &direction,
                                     const float distance) const {
  RealPosition newPosition = realPosition;
  // TODO: get back here if distance is sometimes too big!
  if (distance >= 2.0f) {
    m_logger->logError("Distance to move entity in '" +
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
      m_logger->logError("Tested tile position " + realPosition.toString() +
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
  if (std::all_of(m_level->getPacMans().cbegin(), m_level->getPacMans().cend(),
                  [](const auto &pacman) -> bool {
                    return pacman->getEntityState() == EntityState::DEAD;
                  })) {
    m_logger->logInfo("Game Over!, all players are dead.");
    return true;
  }

  // Check for remaining pellets
  if (m_level->getNumberOfFood() == 0) {
    m_logger->logInfo("Game Over!, all the food is gone.");
    return true;
  }

  return false;
}

} // namespace GameLogic
} // namespace PacMan
