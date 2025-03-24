//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <chrono>
#include <iostream>
#include <thread>
#include <zmq.hpp>

#include "GameMessages/MessageHeader.h"
#include "Server/Server.h"

namespace PacMan {
namespace Server {

int Server::main() {
  m_logger.logInfo("Starting server under ip '" + m_hostIp + "'");

  auto listenerThread = std::thread([this]() { this->listenForPlayers(); });

  m_serverWorking = true;
  while (m_serverWorking) {
    // Sleep if no game is running, separate thread is waiting for players
    while (m_serverWorking && !m_gameRunner) {
      m_logger.logInfo("Waiting for connection...");
      std::unique_lock<std::mutex> lock(m_startGameMutex);
      m_startGameCondition.wait(lock, [this]() {
        return (!m_serverWorking) || this->m_gameRunner != nullptr;
      });
    }
  }

  m_logger.logDebug("Waiting for listener thread to join...");
  m_listenToPlayers = false;
  listenerThread.join();
  m_logger.logDebug("Listener thread joined");

  m_logger.logInfo("Server under ip '" + m_hostIp + "' shutting down...");
  return 0;
}

void Server::listenForPlayers() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, zmq::socket_type::rep);
  socket.bind(m_clientServerReqReplSocketAddr);

  m_listenToPlayers = true;
  while (m_listenToPlayers) {
    // Receive request
    zmq::message_t request;
    zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);

    auto header =
        static_cast<GameLogic::GameMessages::MessageHeader *>(request.data());
    if (header->source != GameLogic::GameMessages::MessageSource::CLIENT) {
      m_logger.logError("Received a message from another server.");
      continue;
    }

    if (header->event ==
        GameLogic::GameMessages::MessageType::CONNECTION_MESSAGE) {
      auto *connHeader =
          static_cast<GameLogic::GameMessages::ConnectionMessageHeader *>(
              request.data());
      zmq::message_t reply = m_reqRespHandler.handleReqRepl(connHeader);
      socket.send(reply, zmq::send_flags::none);
    }
  }
}

void Server::startGame() {
  // TODO: Implement starting a game...
}

void Server::loadGame(const std::string &boardName) {
  m_logger.logInfo("Loading game from the board '" + boardName + "'");
  m_gameRunner = m_gameHandler.loadGame("Board1.txt");
  if (m_gameRunner == nullptr) {
    m_logger.logError("Failed to create a game runner from board '" +
                      boardName + "'");
  }
}

void Server::shutdown() {
  m_logger.logWarning("Shutting down server...");
  m_gameRunner.release();
  m_serverWorking = false;
  m_startGameCondition.notify_all();
  m_listenToPlayers = false;
}

} // namespace Server
} // namespace PacMan
