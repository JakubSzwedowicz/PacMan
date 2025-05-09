//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef SERVER_H
#define SERVER_H

#include <string>

#include "GameLogic/GameHandler.h"
#include "GameLogic/GameRunner.h"
// #include "GameMessagesHandlers/ReqRespHandler.h"
#include "Entities/PacMan.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace Server {

class Server {
public:
  Server(GameEvents::GameEventsManager &gameEventsManager);
  int main();
  int debugMain();
  void listenForPlayers();
  void startGame();
  void loadGame(const std::string &boardName);

  // Call when server needs to immediately shut down
  void shutdown();

private:
  void debugListenForPlayers();
  // friend GameMessagesHandlers::ReqRespHandler;

private:
  Utils::Logger m_logger = Utils::Logger("Server", Utils::LogLevel::INFO);
  const std::string m_hostIp = "127.0.0.1";
  const std::string m_clientServerReqReplSocketAddr =
      "tcp://" + m_hostIp + ":5555";
  PacMan::GameLogic::GameHandler m_gameHandler;
  std::unique_ptr<PacMan::GameLogic::GameSession> m_gameSession = nullptr;
  std::thread m_listenerThread;

  // synchronization primitives
  std::atomic_bool m_listenToPlayers = false;
  std::atomic_bool m_serverWorking = false;
  std::mutex m_startGameMutex;
  std::condition_variable m_startGameCondition;

  // Messages Handlers
  GameEvents::GameEventsManager &m_gameEventsManager;
  // GameMessagesHandlers::ReqRespHandler m_reqRespHandler = {this};
};

} // namespace Server
} // namespace PacMan

#endif // SERVER_H
