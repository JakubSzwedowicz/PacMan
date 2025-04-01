//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H


#include <memory>
#include <string>
#include <vector>

#include "Entities/Ghost.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameRunner.h"
#include "Utils/Logger.h"
#include "GameSession.h"

namespace PacMan {
namespace GameLogic {

class GameHandler {
public:
  GameHandler(GameEvents::GameEventsManager &gameEventsManager);
  std::unique_ptr<pacMan::GameLogic::GameSession> loadGame(const std::string &boardName);

private:
  GameObjects::Entities::GhostTypeToGhostStateToGhostStrategies_t
  getGhostStrategies(const GameObjects::Level &level) const;

private:
  mutable Utils::Logger m_logger =
      Utils::Logger{"GameHandler", Utils::LogLevel::INFO};
  GameEvents::GameEventsManager &m_gameEventsManager;
  int m_nextGameId = 0;
  // TOOD: Add ResoruceManager to handle this type of stuff
  const std::vector<std::string> m_possibleBoards = {"Board1.txt"};
};

} // namespace GameLogic
} // namespace PacMan

#endif // GAMEHANDLER_H
