//
// Created by jakubszwedowicz on 3/31/25.
//

#include "GameLogic/GameSession.h"

#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameRunner.h"
#include "GameObjects/Level.h"

namespace PacMan::GameLogic {

GameSession::GameSession(
    int gameSessionId, bool isClient,
    std::unique_ptr<GameObjects::Level> level,
    GameEvents::GameEventsManager &gameEventsManager)
    : ISubscriber(&gameEventsManager.getGameSessionEventPublisher()),
      m_gameSessionId(gameSessionId), m_isClient(isClient),
      m_gameSessionStatus(GameLogic::GameSessionStatus::WAITING),
      // Clients do not run the game. They only receive game events. Though they still have the GameSession
      m_gameRunner(
          m_isClient ? nullptr
                     : std::make_unique<GameLogic::GameRunner>(
                           gameSessionId, std::move(level), gameEventsManager)),
      m_gameEventsManager(gameEventsManager) {}

void GameSession::startSession() {}
void GameSession::callback(const GameEvents::GameSessionEvent &event) {
  switch (event.gameSessionEventType) {
    // TODO: Implement these!
  case GameEvents::GameSessionEventType::CREATE_SESSION:
    break;
    // case GameEvents::GameSessionEventType::SESSION_INFO:
    //   break;
    // case GameEvents::GameSessionEventType::PLAYER_UPDATE:
    //   break;
    // case GameEvents::GameSessionEventType::DESTROY_SESSION:
    //   break;
  }
}

} // namespace pacMan