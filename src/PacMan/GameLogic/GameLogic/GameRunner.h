//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef GAMERUNNER_H
#define GAMERUNNER_H

#include <memory>

#include "GameObjects/Level.h"

namespace PacMan {
namespace GameLogic {

class GameRunner {
public:
  GameRunner(std::unique_ptr<GameObjects::Level> level);
  bool startGame();

private:
  std::unique_ptr<GameObjects::Level> m_level;
};

} // namespace GameLogic
} // namespace PacMan

#endif // GAMERUNNER_H
