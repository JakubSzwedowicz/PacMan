//
// Created by jakubszwedowicz on 3/31/25.
//

#ifndef GAMESESSION_H
#define GAMESESSION_H
#include <memory>

#include "GameEvents/GameEvents.h"
#include "GameEvents/GameSessionEvents.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameObjects/Level.h"
#include "GameRunner.h"
#include "GameSessionStatus.h"

namespace PacMan::GameLogic {

/**
 * @brief This class manages game session which is tracking if players are ready
 * to change the game to RUNNING. Could also manage shutting the game down or
 * PAUSING it.
 */
class GameSession
    : public Utils::ISubscriber<GameEvents::GameSessionEvent> {
public:
  GameSession(int gameSessionId, bool isClient,
              std::unique_ptr<GameObjects::Level> level,
              GameEvents::GameEventsManager &gameEventsManager);
  void startSession();
  void callback(const GameEvents::GameSessionEvent &event) override;

  int getGameSessionId() const { return m_gameSessionId; }
  [[nodiscard]] const GameLogic::GameRunner &getGameRunner() const {
    return *m_gameRunner;
  }

private:
  int m_gameSessionId;
  const bool m_isClient;
  GameLogic::GameSessionStatus m_gameSessionStatus;
  std::unique_ptr<GameLogic::GameRunner> m_gameRunner;
  GameEvents::GameEventsManager &m_gameEventsManager;
};

} // namespace PacMan::GameLogic

#endif // GAMESESSION_H
