#include <csignal>
#include <memory>

#include "Entities/PacMan.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameHandler.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"
#include "Utils/Signals.h"

class EntityEventsPublisher final
    : public PacMan::Utils::IPublisher<PacMan::GameEvents::EntityEvent> {};

int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  auto gameEventsManager = PacMan::GameEvents::GameEventsManager{
      std::make_unique<EntityEventsPublisher>()};
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