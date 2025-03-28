//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "Entities/Entity.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "LevelState.h"
#include "Utils/Logger.h"

#include <memory>
#include <vector>

namespace PacMan {
namespace GameObjects {

class Level {
public:
  using Board_t = std::vector<std::vector<std::unique_ptr<Entities::Entity>>>;
  Level() = default;

  void swapEntities(const Entities::TilePosition &pos1,
                           const Entities::TilePosition &pos2);
  std::unique_ptr<Entities::Entity>
  removeEntity(const Entities::TilePosition &pos);

  std::vector<Entities::TilePosition>
  getValidAdjacentPositions(const Entities::TilePosition &pos) const;

  bool isReady() const { return m_levelState == LevelState::READY; }
  bool isTilePositionValid(const Entities::TilePosition &pos) const {
    return pos.x >= 0 && static_cast<size_t>(pos.x) < getWidth() && pos.y >= 0 &&
           static_cast<size_t>(pos.y)  < getHeight();
  }
  bool setBoard(std::unique_ptr<Board_t> board);
  const Board_t::value_type::value_type &
  getEntityOnTile(const Entities::TilePosition &tilePosition) const {
    return m_board[tilePosition.y][tilePosition.x];
  }

  Board_t &getBoard() { return m_board; }
  size_t getWidth() const { return m_board.front().size(); }
  size_t getHeight() const { return m_board.size(); }
  std::vector<Entities::PacMan *> &getPacmans() { return m_pacMans; }
  std::vector<Entities::Ghost *> &getGhosts() { return m_ghosts; }
  int getNumberOfFood() const { return m_numberOfFood; }

private:
  Board_t::value_type::value_type &
  getMutableEntityOnTile(const Entities::TilePosition &tilePosition) {
    return m_board[tilePosition.y][tilePosition.x];
  }

  Utils::Logger m_logger = Utils::Logger("Level", Utils::LogLevel::INFO);
  LevelState m_levelState = LevelState::NOT_READY;
  Board_t m_board;
  int m_numberOfFood = 0;
  std::vector<Entities::Ghost *> m_ghosts;
  std::vector<Entities::PacMan *> m_pacMans;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVEL_H
