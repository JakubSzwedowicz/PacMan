//
// Created by jakubszwedowicz on 2/28/25.
//

#include <algorithm>
#include <utility>

#include "Entities/EntityType.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameObjects/Level.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameObjects {

Level::Level(Board_t &&board)
    : m_logger(std::make_unique<Utils::Logger>("Level", Utils::LogLevel::INFO)),
      m_board(std::move(board)) {
  verifyBoard();
}

bool Level::verifyBoard() {
  m_levelState = LevelState::NOT_READY;
  for (int row = 0; row < m_board.size(); row++) {
    for (int col = 0; col < m_board[row].size(); col++) {
      const auto &cell = m_board[row][col];
      if (cell == Entities::EntityType::PELLET) {
        m_numberOfFood++;
      } else if (cell == Entities::EntityType::WALL) {
        // Do nothing...
      } else if (cell == Entities::EntityType::EMPTY) {
        // If found best position for Blinky
        if ((row < m_ghostScatterPositions.blinkyScatterPositionTopRight.y ||
             col > m_ghostScatterPositions.blinkyScatterPositionTopRight.x) &&
            (row <= m_ghostScatterPositions.blinkyScatterPositionTopRight.y &&
             col >= m_ghostScatterPositions.blinkyScatterPositionTopRight.x))
          m_ghostScatterPositions.blinkyScatterPositionTopRight = {col, row};

        // If found best position for Clyde
        if ((row > m_ghostScatterPositions.clydeScatterPositionBottomLeft.y ||
             col < m_ghostScatterPositions.clydeScatterPositionBottomLeft.x) &&
            (row >= m_ghostScatterPositions.clydeScatterPositionBottomLeft.y &&
             col <= m_ghostScatterPositions.clydeScatterPositionBottomLeft.x))
          m_ghostScatterPositions.clydeScatterPositionBottomLeft = {col, row};

        // If found best position for Inky
        if ((row > m_ghostScatterPositions.inkyScatterPositionBottomRight.y ||
             col > m_ghostScatterPositions.inkyScatterPositionBottomRight.x) &&
            (row >= m_ghostScatterPositions.inkyScatterPositionBottomRight.y &&
             col >= m_ghostScatterPositions.inkyScatterPositionBottomRight.x))
          m_ghostScatterPositions.inkyScatterPositionBottomRight = {col, row};

        // If found best position for Pinky
        if ((row < m_ghostScatterPositions.pinkyScatterPositionTopLeft.y ||
             col < m_ghostScatterPositions.pinkyScatterPositionTopLeft.x) &&
            (row <= m_ghostScatterPositions.pinkyScatterPositionTopLeft.y &&
             col <= m_ghostScatterPositions.pinkyScatterPositionTopLeft.x))
          m_ghostScatterPositions.pinkyScatterPositionTopLeft = {col, row};
      } else {
        m_logger->logError("Not a valid entity type: " + toString(cell));
      }
    }
  }
  return true;
}

std::vector<Entities::TilePosition>
Level::getValidAdjacentPositions(const Entities::TilePosition &pos) const {
  std::vector<Entities::TilePosition> result;
  result.reserve(4);

  std::vector<Entities::TilePosition> offsets = {
      {-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (const auto &offset : offsets) {
    Entities::TilePosition neighboringPos = (pos + offset);
    if (isTilePositionValid(neighboringPos)) {
      result.push_back(neighboringPos);
    }
  }
  return result;
}

Entities::Ghost *Level::getGhostOrAnyButNot(const Entities::Ghost &notThisGhost,
                                            Entities::GhostType ghostType) const {
  // This can be done in single pass but whatever...
  auto ghost = std::find_if(
      m_ghosts.cbegin(), m_ghosts.cend(),
      [ghostType](const std::unique_ptr<Entities::Ghost> &pos) { return pos->getGhostType() == ghostType; });
  if (ghost != m_ghosts.end())
    return ghost->get();
  if (!m_ghosts.empty()) {
    ghost = std::find_if(
        m_ghosts.cbegin(), m_ghosts.cend(),
        [&notThisGhost](const std::unique_ptr<Entities::Ghost> &pos) { return pos.get() != &notThisGhost; });
    return (ghost != m_ghosts.end()) ? ghost->get() : nullptr;
  }
  return nullptr;
}

void Level::setPacMans(Pacmans_t &&pacmans) {
  m_pacMans = std::move(pacmans);
  processSetPacMandAndGhosts();
}
void Level::setGhosts(Ghosts_t &&ghosts) {
  m_ghosts = std::move(ghosts);
  processSetPacMandAndGhosts();
}

[[nodiscard]] int Level::getNumberOfFood() const { return m_numberOfFood; }
[[nodiscard]] Level::GhostScatterPositions
Level::getGhostScatterPositions() const {
  return m_ghostScatterPositions;
}
[[nodiscard]] Entities::TilePosition
Level::getScatteringPositionOfGhost(Entities::GhostType ghostType) const {
  switch (ghostType) {
  case Entities::GhostType::BLINKY:
    return m_ghostScatterPositions.blinkyScatterPositionTopRight;
  case Entities::GhostType::CLYDE:
    return m_ghostScatterPositions.clydeScatterPositionBottomLeft;
  case Entities::GhostType::INKY:
    return m_ghostScatterPositions.inkyScatterPositionBottomRight;
  case Entities::GhostType::PINKY:
    return m_ghostScatterPositions.pinkyScatterPositionTopLeft;
  default:
    m_logger->logError("Returned ghost type is invalid" +
                       Entities::toString(ghostType));
    return m_ghostScatterPositions.clydeScatterPositionBottomLeft;
  }
}

void Level::processSetPacMandAndGhosts() {
  if (m_pacMans.empty() || m_ghosts.empty()) {return;}
  m_movingEntities.clear();
  for (const auto& pacMan : m_pacMans) {
    m_movingEntities.push_back(pacMan.get());
    setEntityOnTile(pacMan->getStartingPosition(), Entities::EntityType::EMPTY);
  }
  for (const auto& ghost : m_ghosts) {
    m_movingEntities.push_back(ghost.get());
    setEntityOnTile(ghost->getStartingPosition(), Entities::EntityType::EMPTY);
  }
  m_levelState = LevelState::READY;
}

// bool Level::setBoard(Board_t &&board) {
//   m_board = std::move(board);
//   updateState(InitializationState::BOARD_SET);
//   return true;
// }
// bool Level::setPacMans(
//     std::vector<std::unique_ptr<Entities::PacMan>> &&pacMans) {
//   m_pacMans = std::move(pacMans);
//   updateState(InitializationState::PACMANS_SET);
//   return true;
// }
// bool Level::setGhosts(std::vector<std::unique_ptr<Entities::Ghost>> &&ghosts)
// {
//   m_ghosts = std::move(ghosts);
//   updateState(InitializationState::GHOSTS_SET);
//   return true;
// }
//
// void Level::updateState(const InitializationState state) {
//   m_initializationStateMask |=
//       static_cast<std::underlying_type_t<InitializationState>>(state);
//
//   if (m_initializationStateMask == s_initializationStateReadyMask) {
//     m_levelState = LevelState::READY;
//   }
// }

} // namespace GameObjects
} // namespace PacMan