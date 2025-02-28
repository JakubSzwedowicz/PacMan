//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "Entity.h"
#include "Ghost.h"
#include "PacMan.h"

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
  // bool setPacMans(std::vector<std::unique_ptr<Entities::PacMan>> &&pacMans);
  // bool setGhosts(std::vector<std::unique_ptr<Entities::Ghost>> &&ghosts);

private:
  // enum class InitializationState : uint8_t {
  //   BOARD_SET = 1 << 1,
  //   PACMANS_SET = 1 << 2,
  //   GHOSTS_SET = 1 << 3
  // };
  //
  // void updateState(const InitializationState state);
  //
  // static constexpr uint8_t s_initializationStateReadyMask =
  //     static_cast<std::underlying_type_t<InitializationState>>(
  //         InitializationState::BOARD_SET) |
  //     static_cast<std::underlying_type_t<InitializationState>>(
  //         InitializationState::PACMANS_SET) |
  //     static_cast<std::underlying_type_t<InitializationState>>(
  //         InitializationState::GHOSTS_SET);

  // uint8_t m_initializationStateMask = 0;

  LevelState m_levelState = LevelState::NOT_READY;
  Board_t m_board;
  std::vector<Entities::Ghost *> m_ghosts;
  std::vector<Entities::PacMan *> m_pacMans;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVEL_H
