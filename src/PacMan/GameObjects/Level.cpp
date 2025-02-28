//
// Created by jakubszwedowicz on 2/28/25.
//

#include "Level.h"

namespace PacMan {
namespace GameObjects {

bool Level::setBoard(std::unique_ptr<Board_t> board) {
  m_levelState = LevelState::NOT_READY;
  m_board = std::move(*board);
  for (const auto &row : m_board) {
    for (const auto &cell : row) {
      if (cell->getEntityType() == Entities::EntityType::PAC_MAN) {
        auto *pacMan = dynamic_cast<Entities::PacMan *>(cell.get());
        if (pacMan == nullptr) {
          return false;
        }
        m_pacMans.push_back(pacMan);
      } else if (cell->getEntityType() == Entities::EntityType::GHOST) {
        auto *ghost = dynamic_cast<Entities::Ghost *>(cell.get());
        if (ghost == nullptr) {
          return false;
        }
        m_ghosts.push_back(ghost);
      }
    }
  }

  m_levelState = LevelState::READY;
  return true;
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