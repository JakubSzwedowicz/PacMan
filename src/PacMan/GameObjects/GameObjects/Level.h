//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "Entities/Ghost.h"
#include "Entities/IEntity.h"
#include "Entities/PacMan.h"
#include "LevelState.h"
#include "Utils/Logger.h"

#include <memory>
#include <vector>

namespace PacMan {
namespace GameObjects {

class Level {
public:
  using Board_t = std::vector<std::vector<std::unique_ptr<Entities::IEntity>>>;
  Level() = default;

  void swapEntities(Entities::Position pos1, Entities::Position pos2);
  std::unique_ptr<Entities::IEntity> removeEntity(Entities::Position pos);

  std::vector<Entities::Position>
  getValidAdjacentPositions(const Entities::Position &pos) const;

  bool isReady() const { return m_levelState == LevelState::READY; }
  bool setBoard(std::unique_ptr<Board_t> board);
  Board_t &getBoard() { return m_board; }
  size_t getWidth() const { return m_board.front().size(); }
  size_t getHeight() const { return m_board.size(); }
  std::vector<Entities::PacMan *> &getPacmans() { return m_pacMans; }
  std::vector<Entities::Ghost *> &getGhosts() { return m_ghosts; }
  int getNumberOfFood() const { return m_numberOfFood; }

private:
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
