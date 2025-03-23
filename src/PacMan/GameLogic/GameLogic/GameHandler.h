//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include <string>
#include <vector>

namespace PacMan {
namespace GameLogic {

class GameHandler {
public:
  GameHandler() = default;

private:
  // TOOD: Add ResoruceManager to handle this type of stuff
  const std::vector<std::string> m_possibleBoards = {"Board1.txt"};
};

} // namespace GameLogic
} // namespace PacMan

#endif // GAMEHANDLER_H
