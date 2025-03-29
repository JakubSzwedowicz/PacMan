#include <csignal>
#include <memory>

#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameHandler.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"
#include "Utils/Signals.h"

class EntityEventsPublisher final : public PacMan::Utils::IPublisher<PacMan::GameEvents::EntityEvent> {};

int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  auto gameEventsManager = PacMan::GameEvents::GameEventsManager{std::make_unique<EntityEventsPublisher>()};
  PacMan::GameLogic::GameHandler gameHandler{gameEventsManager};

  return 0;
}