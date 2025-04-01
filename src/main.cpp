#include <csignal>
#include <memory>

#include "Entities/PacMan.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameEvents/GameEvents.h"
#include "GameLogic/GameHandler.h"
#include "GameLogic/GameStatus.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"
#include "Utils/Signals.h"
#include "GameLogic/GameRunner.h"
#include "GameLogic/GameSession.h"
#include "GameObjects/Level.h"

using namespace PacMan::GameEvents;

class EntityEventsPublisher final
    : public GameEventsManager::EntityEventPublisher_t {};
class GameEventsPublisher final
    : public GameEventsManager::GameEventPublisher_t {};
class GameSessionEventsPublisher final
    : public GameEventsManager::GameSessionEventPublisher_t {};

int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  auto gameEventsManager = GameEventsManager{
      std::make_unique<EntityEventsPublisher>(),
      std::make_unique<GameEventsPublisher>(),
      std::make_unique<GameSessionEventsPublisher>(),
  };
  PacMan::GameLogic::GameHandler gameHandler{gameEventsManager};
  auto gameSession = gameHandler.loadGame("Board1.txt");

  if (gameSession) {
    gameEventsManager.getGameEventPublisher().publish(GameStatusChanged(
    gameSession->getGameSessionId(), PacMan::GameLogic::GameStatus::CREATING));
    gameSession->startSession();
    while (true) {
      gameSession->getGameRunner().printToCLI();
    }
  }

  return 0;
}