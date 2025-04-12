//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>

#include "GameLogic/GameHandler.h"
#include "Utils/Logger.h"

#include <GameEventsManager/BasicGameEventsPublishers.h>

namespace PacMan {

namespace GameLogic {
class GameRunner;
}

namespace Client {

class Client {
public:
  Client(GameEvents::GameEventsManager &gameEventsManager);
  int main();
  void startSinglePlayerGame();

private:
  Utils::Logger m_logger = Utils::Logger("Client", Utils::LogLevel::INFO);
  const std::string m_hostIp = "127.0.0.1";
  //  const std::string m_clientServerReqReplSocketAddr =
  //      "tcp://" + m_hostIp + ":5555";
  PacMan::GameLogic::GameHandler m_gameHandler;
  std::unique_ptr<PacMan::GameLogic::GameRunner> m_gameRunner = nullptr;
  GameEvents::GameEventsManager m_gameEventsManager =
};

// synchronization primitives
std::atomic_bool m_listenToPlayers = false;
std::atomic_bool m_serverWorking = false;
std::mutex m_startGameMutex;
std::condition_variable m_startGameCondition;
};
} // namespace Client
} // namespace PacMan
#endif // CLIENT_H
