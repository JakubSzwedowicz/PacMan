#include "Client/Client/Client.h"

#include <csignal>
#include <memory>

#include "Entities/PacMan.h"
#include "GameEvents/GameEvents.h"
#include "GameEventsManager/BasicGameEventsPublishers.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameLogic/GameHandler.h"
#include "GameLogic/GameRunner.h"
#include "GameLogic/GameSession.h"
#include "GameLogic/GameStatus.h"
#include "GameObjects/Level.h"
#include "Server/Server/Server.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"
#include "Utils/Signals.h"

using namespace PacMan::GameEvents;

template <typename Iter>
auto acc(Iter begin, Iter end) {
  using ValueType = decltype(*begin);
  ValueType result = ValueType();
  while (begin != end) {
    result += (*begin);
    ++begin;
  }

  return result;
}

int main() {
  std::vector<int> v = {1,2,3,};
  std::cout << acc(v.begin(), v.end()) << std::endl;
  // PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  // std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);
  //
  // auto gameEventsManager = GameEventsManager{
  //     std::make_unique<EntityEventsPublisher>(),
  //     std::make_unique<GameEventsPublisher>(),
  //     std::make_unique<GameSessionEventsPublisher>(),
  // };
  //
  // PacMan::Client::Client client(gameEventsManager);
  // PacMan::Server::Server server(gameEventsManager);
  //
  // PacMan::GameLogic::GameHandler gameHandler{gameEventsManager};
  // auto gameSession = gameHandler.loadGame("Board1.txt");
  //
  // if (gameSession) {
  //   gameEventsManager.getGameEventPublisher().publish(
  //       GameStatusChanged(gameSession->getGameSessionId(),
  //                         PacMan::GameLogic::GameStatus::CREATING));
  //   gameSession->startSession();
  //   while (true) {
  //     gameSession->getGameRunner().printToCLI();
  //   }
  // }

  return 0;
}