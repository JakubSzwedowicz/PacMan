//
// Created by jakubszwedowicz on 3/31/25.
//

#include "GameLogic/GameSession.h"

#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameRunner.h"
#include "GameObjects/Level.h"

namespace pacMan {
namespace GameLogic {

GameSession::GameSession(
    int gameSessionId, bool isClient,
    std::unique_ptr<PacMan::GameObjects::Level> level,
    PacMan::GameEvents::GameEventsManager &gameEventsManager)
    : ISubscriber(&gameEventsManager.getGameSessionEventPublisher()),
      m_gameSessionId(gameSessionId), m_isClient(isClient),
      m_gameRunner(nullptr),
      m_gameSessionStatus(PacMan::GameLogic::GameSessionStatus::WAITING),
      m_gameEventsManager(gameEventsManager) {
  if (!m_isClient) {
    m_gameRunner = std::make_unique<PacMan::GameLogic::GameRunner>(
        gameSessionId, std::move(level), gameEventsManager);
  }
}

void GameSession::startSession() {}
void GameSession::callback(const PacMan::GameEvents::GameSessionEvent &event) {
  switch (event.gameSessionEventType) {
    // TODO: Implement these!
  case PacMan::GameEvents::GameSessionEventType::CREATE_SESSION:
    break;
    // case PacMan::GameEvents::GameSessionEventType::SESSION_INFO:
    //   break;
    // case PacMan::GameEvents::GameSessionEventType::PLAYER_UPDATE:
    //   break;
    // case PacMan::GameEvents::GameSessionEventType::DESTROY_SESSION:
    //   break;
  }
}

} // namespace GameLogic
} // namespace pacMan