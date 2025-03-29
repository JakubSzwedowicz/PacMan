//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVEL_H
#define LEVEL_H

#include <memory>
#include <vector>

#include "Entities/Entity.h"
#include "Entities/EntityType.h"
#include "LevelState.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameObjects {

namespace Entities {
class PacMan;
class Ghost;
}; // namespace Entities

class Level {
public:
  using Board_t = std::vector<std::vector<Entities::EntityType>>;
  using Pacmans_t = std::vector<std::unique_ptr<Entities::PacMan>>;
  using Ghosts_t = std::vector<std::unique_ptr<Entities::Ghost>>;
  Level(Board_t &&board, Pacmans_t &&pacmans, Ghosts_t &&ghosts);

  bool verifyBoard();

  [[nodiscard]] std::vector<Entities::TilePosition>
  getValidAdjacentPositions(const Entities::TilePosition &pos) const;

  [[nodiscard]] bool isReady() const {
    return m_levelState == LevelState::READY;
  }
  [[nodiscard]] bool
  isTilePositionValid(const Entities::TilePosition &pos) const {
    return pos.x >= 0 && static_cast<size_t>(pos.x) < getWidth() &&
           pos.y >= 0 && static_cast<size_t>(pos.y) < getHeight() &&
           m_board[pos.y][pos.x] != Entities::EntityType::WALL;
  }

  [[nodiscard]] Board_t::value_type::value_type
  getEntityOnTile(const Entities::TilePosition &tilePosition) const {
    return m_board[tilePosition.y][tilePosition.x];
  }
  void setEntityOnTile(const Entities::TilePosition &tilePosition,
                       Entities::EntityType entityType) {
    m_board[tilePosition.y][tilePosition.x] = entityType;
  }

  [[nodiscard]] const Board_t &getBoard() const { return m_board; }
  [[nodiscard]] size_t getWidth() const { return m_board.front().size(); }
  [[nodiscard]] size_t getHeight() const { return m_board.size(); }
  Pacmans_t &getPacmans() { return m_pacMans; }
  Ghosts_t &getGhosts() { return m_ghosts; }
  [[nodiscard]] int getNumberOfFood() const { return m_numberOfFood; }

private:
  Board_t::value_type::value_type
  getMutableEntityOnTile(const Entities::TilePosition &tilePosition) {
    return m_board[tilePosition.y][tilePosition.x];
  }

private:
  Utils::Logger m_logger = Utils::Logger("Level", Utils::LogLevel::INFO);
  LevelState m_levelState = LevelState::NOT_READY;
  Board_t m_board;
  Pacmans_t m_pacMans;
  Ghosts_t m_ghosts;

  int m_numberOfFood = 0;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVEL_H
