//
// Created by jakubszwedowicz on 2/28/25.
//

#include <algorithm>
#include <utility>

#include "Entities/EntityType.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameObjects {

Level::Level(Board_t &&board, Pacmans_t &&pacmans, Ghosts_t &&ghosts)
    : m_board(std::move(board)), m_pacMans(std::move(pacmans)),
      m_ghosts(std::move(ghosts)) {
  verifyBoard();
}

bool Level::verifyBoard() {
  m_levelState = LevelState::NOT_READY;
  for (const auto &row : m_board) {
    for (const auto &cell : row) {
      if (cell == Entities::EntityType::PELLET) {
        m_numberOfFood++;
      } else if (cell == Entities::EntityType::WALL) {
        // Do nothing...
      } else if (cell == Entities::EntityType::EMPTY) {
        // Empty...
      } else {
        m_logger.logError("Not a valid entity type: " + toString(cell));
      }
    }
  }

  // What a smart thing!!!
  auto pred = [this](const auto &entity) -> bool {
    return getEntityOnTile(entity->getTilePosition()) !=
           Entities::EntityType::EMPTY;
  };
  if (std::any_of(m_pacMans.cbegin(), m_pacMans.cend(), pred) ||
      std::any_of(m_ghosts.cbegin(), m_ghosts.cend(), pred)) {
    return false;
  }

  m_levelState = LevelState::READY;
  return true;
}

std::vector<Entities::TilePosition>
Level::getValidAdjacentPositions(const Entities::TilePosition &pos) const {
  std::vector<Entities::TilePosition> result;

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