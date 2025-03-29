//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include <memory>
#include <string>
#include <vector>

#include "Utils/Logger.h"

namespace PacMan::GameLogic {
class GameRunner;
}
namespace PacMan {
namespace GameLogic {

class GameHandler {
public:
  GameHandler() = default;
  std::unique_ptr<GameRunner> loadGame(const std::string &boardName);

private:
  Utils::Logger m_logger = Utils::Logger{"GameHandler", Utils::LogLevel::INFO};
  int m_nextGameId = 0;
  // TOOD: Add ResoruceManager to handle this type of stuff
  const std::vector<std::string> m_possibleBoards = {"Board1.txt"};
};

} // namespace GameLogic
} // namespace PacMan

#endif // GAMEHANDLER_H
