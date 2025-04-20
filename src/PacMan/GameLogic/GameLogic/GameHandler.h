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
#include "GameSession.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameLogic {

class GameHandler {
public:
  GameHandler(GameEvents::GameEventsManager &gameEventsManager, const bool isClient);
  std::unique_ptr<GameLogic::GameSession>
  loadGame(const std::string &boardName, const int gameSessionId, const int numberOfPlayers);

  /*
   * @brief Returns a unique game session id. It will never return the same GameSessionId twice.
   * @return Unique game session id.
   */
  int getUniqueGameSessionId();

private:
  GameObjects::Entities::GhostTypeToGhostStateToGhostStrategies_t
  getGhostStrategies(const GameObjects::Level &level) const;

private:
  void ensureNumberOfPlayers(Level& level, const int numberOfPlayers) const;

  mutable Utils::Logger m_logger =
      Utils::Logger{"GameHandler", Utils::LogLevel::INFO};
  GameEvents::GameEventsManager &m_gameEventsManager;
  const bool m_isClient;
  int m_nextGameSessionId = 0;
  // TOOD: Add ResoruceManager to handle this type of stuff
  const std::vector<std::string> m_possibleBoards = {"Board1.txt"};
};

} // namespace GameLogic
} // namespace PacMan

#endif // GAMEHANDLER_H
