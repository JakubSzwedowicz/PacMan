#include <csignal>
#include <memory>

#include "Entities/PacMan.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameHandler.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"
#include "Utils/Signals.h"

using namespace PacMan::GameEvents;

class EntityEventsPublisher final
    : public GameEventsManager::EntityEventPublisher_t {};
class GameEventsPublisher final
    : public GameEventsManager::GameEventPublisher_t {};


int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  auto gameEventsManager = GameEventsManager{
      std::make_unique<EntityEventsPublisher>(), std::make_unique<GameEventsPublisher>()};
  PacMan::GameLogic::GameHandler gameHandler{gameEventsManager};
  auto gameRunner = gameHandler.loadGame("Board1.txt");
  if (gameRunner) {
    gameRunner->startGame();
    while (true) {
      gameRunner->printToCLI();
    }
  }

  return 0;
}