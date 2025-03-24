//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "Entities/Ghost.h"
#include "Entities/IEntity.h"
#include "Entities/PacMan.h"
#include "LevelState.h"

#include <memory>
#include <vector>

namespace PacMan {
namespace GameObjects {

class Level {
public:
  using Board_t = std::vector<std::vector<std::unique_ptr<Entities::IEntity>>>;
  Level() = default;

  bool isReady() const { return m_levelState == LevelState::READY; }

  bool setBoard(std::unique_ptr<Board_t> board);

  const Board_t &getBoard() const { return m_board; }

private:
  LevelState m_levelState = LevelState::NOT_READY;
  Board_t m_board;
  std::vector<Entities::Ghost *> m_ghosts;
  std::vector<Entities::PacMan *> m_pacMans;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVEL_H
