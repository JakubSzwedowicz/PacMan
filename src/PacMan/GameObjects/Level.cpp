//
// Created by jakubszwedowicz on 2/28/25.
//

#include "GameObjects/Level.h"

#include "Entities/Empty.h"
#include "Entities/EntityType.h"

#include <utility>

namespace PacMan {
namespace GameObjects {

bool Level::setBoard(std::unique_ptr<Board_t> board) {
  m_levelState = LevelState::NOT_READY;
  m_board = std::move(*board);
  for (const auto &row : m_board) {
    for (const auto &cell : row) {
      if (!cell) {
        continue;
      }

      if (cell->getEntityType() == Entities::EntityType::PAC_MAN) {
        auto *pacMan = dynamic_cast<Entities::PacMan *>(cell.get());
        if (pacMan == nullptr) {
          m_logger.logError("Entity could not be cast to PacMan at position " +
                            cell->getTilePosition().toString());
          return false;
        }
        m_pacMans.push_back(pacMan);
      } else if (cell->getEntityType() == Entities::EntityType::GHOST) {
        auto *ghost = dynamic_cast<Entities::Ghost *>(cell.get());
        if (ghost == nullptr) {
          m_logger.logError("Entity could not be cast to Ghost at position " +
                            cell->getTilePosition().toString());
          return false;
        }
        m_ghosts.push_back(ghost);
      } else if (cell->getEntityType() == Entities::EntityType::FOOD) {
        m_numberOfFood++;
      }
    }
  }

  m_levelState = LevelState::READY;
  return true;
}

std::vector<Entities::Position>
Level::getValidAdjacentPositions(const Entities::Position &pos) const {
  std::vector<Entities::Position> result;

  std::vector<Entities::Position> offsets = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (const auto &offset : offsets) {
    int newX = pos.x + offset.x;
    int newY = pos.y + offset.y;
    if (newX >= 0 && newX < getWidth() && newY >= 0 && newY < getHeight()) {
      bool isWall =
          m_board[newY][newX]->getEntityType() == Entities::EntityType::WALL;
      if (!isWall) {
        result.push_back(Entities::Position{newX, newY});
      }
    }
  }
  return result;
}

void Level::swapEntities(Entities::Position pos1, Entities::Position pos2) {
  std::swap(m_board[pos1.y][pos1.x]->getTilePosition(),
            m_board[pos2.y][pos2.x]->getTilePosition());
  std::swap(m_board[pos1.y][pos1.x], m_board[pos2.y][pos2.x]);
}

std::unique_ptr<Entities::IEntity> Level::removeEntity(Entities::Position pos) {
  auto empty = std::make_unique<Entities::Empty>();
  empty->setTilePosition(pos);

  auto result = std::exchange(m_board[pos.y][pos.x], std::move(empty));
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