//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include "GameLogic/GameHandler.h"
#include "GameLogic/GameRunner.h"
#include "GameObjects/LevelBuilderFromFile.h"

#include <memory>

namespace PacMan {
namespace GameLogic {

std::unique_ptr<GameRunner>
GameHandler::loadGame(const std::string &boardName) {
  m_logger.logInfo("Starting a game from board '" + boardName + "'");

  GameObjects::LevelBuilderFromFile builder =
      GameObjects::LevelBuilderFromFile(m_possibleBoards.front());
  auto level = builder.release();

  m_logger.logInfo("Returning a game with id '" + std::to_string(m_nextGameId) +
                   "'");
  return std::make_unique<GameRunner>((m_nextGameId++), std::move(level));
  ;
}

} // namespace GameLogic
} // namespace PacMan