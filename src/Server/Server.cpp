//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <chrono>
#include <iostream>
#include <thread>
#include <zmq.hpp>

// #include "GameMessages/MessageType.h"
#include "Entities/PacMan.h"
#include "Server/Server.h"

namespace PacMan {
namespace Server {

Server::Server(GameEvents::GameEventsManager &gameEventsManager)
    : m_gameHandler(gameEventsManager, false),
      m_gameEventsManager(gameEventsManager) {}

int Server::main() {
  m_logger.logInfo("Starting server under ip '" + m_hostIp + "'");

  m_listenerThread = std::thread([this]() { this->listenForPlayers(); });
  m_serverWorking = true;
  while (m_serverWorking) {
    // Sleep if no game is running, separate thread is waiting for players
    m_logger.logInfo("Waiting for connection...");
    std::unique_lock<std::mutex> lock(m_startGameMutex);
    m_startGameCondition.wait(lock, [this]() {
      return (!m_serverWorking) || this->m_gameSession != nullptr;
    });

    m_logger.logInfo("Starting game session");
    m_gameSession->startSession();
    while (true) {
      m_gameSession->getGameRunner().printToCLI();
    }
  }

  m_logger.logDebug("Waiting for listener thread to join...");
  shutdown();
  m_logger.logDebug("Listener thread joined");

  m_logger.logInfo("Server under ip '" + m_hostIp + "' shutting down...");
  return 0;
}

void Server::listenForPlayers() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, zmq::socket_type::rep);
  socket.bind(m_clientServerReqReplSocketAddr);

  m_listenToPlayers = true;
  int i = 10;
  while (m_listenToPlayers) {
    // Receive request
    zmq::message_t request;
    zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);

    //   auto header =
    //       static_cast<GameLogic::GameMessages::MessageHeader
    //       *>(request.data());
    //   if (header->source != GameLogic::GameMessages::MessageSource::CLIENT) {
    //     m_logger.logError("Received a message from another server.");
    //     continue;
    //   }
    //
    //   if (header->event ==
    //       GameLogic::GameMessages::MessageType::CONNECTION_MESSAGE) {
    //     auto *connHeader =
    //         static_cast<GameLogic::GameMessages::ConnectionMessageHeader *>(
    //             request.data());
    //     zmq::message_t reply = m_reqRespHandler.handleReqRepl(connHeader);
    //     socket.send(reply, zmq::send_flags::none);
    // }
  }
}

void Server::startGame() {
  // TODO: Implement starting a game...
}

void Server::loadGame(const std::string &boardName) {
  m_logger.logInfo("Loading game from the board '" + boardName + "'");
  m_gameSession = m_gameHandler.loadGame(
      "Board1.txt", m_gameHandler.getUniqueGameSessionId(), 1);
  if (m_gameSession == nullptr) {
    m_logger.logError("Failed to create a game session from board '" +
                      boardName + "'");
  }
}

void Server::shutdown() {
  m_logger.logWarning("Shutting down server...");
  m_gameSession.release();
  m_serverWorking = false;
  m_startGameCondition.notify_all();
  m_listenToPlayers = false;
  m_listenerThread.join();
}

int Server::debugMain() {
  m_logger.logInfo("Starting debug server under ip '" + m_hostIp + "'");

  m_listenerThread = std::thread([this]() { this->debugListenForPlayers(); });
  m_serverWorking = true;
  while (m_serverWorking) {
    // Sleep if no game is running, separate thread is waiting for players
    m_logger.logInfo("Waiting for connection...");
    std::unique_lock<std::mutex> lock(m_startGameMutex);
    m_startGameCondition.wait(lock, [this]() {
      return (!m_serverWorking) || this->m_gameSession != nullptr;
    });

    m_logger.logInfo("Starting game session");
    m_gameSession->startSession();
    while (true) {
      m_gameSession->getGameRunner().printToCLI();
    }
  }

  m_logger.logDebug("Waiting for listener thread to join...");
  shutdown();
  m_logger.logDebug("Listener thread joined");

  m_logger.logInfo("Debug Server under ip '" + m_hostIp + "' shutting down...");
}
void Server::debugListenForPlayers() {
  m_listenToPlayers = true;
  int i = 10;
  while (m_listenToPlayers) {
    if (i > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      i--;
    }
    if (i == 0) {
      m_logger.logInfo("Mocking request to create a game from the player, "
                       "loading Board1.txt");
      loadGame("Board1.txt");
      m_startGameCondition.notify_one();
      i--;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
}

} // namespace Server
} // namespace PacMan
