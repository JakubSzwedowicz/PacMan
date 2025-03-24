//
// Created by jakubszwedowicz on 3/24/25.
//

#include "GameMessagesHandlers/ReqRespHandler.h"
#include "Server/Server.h"

#include <zmq.hpp>

namespace PacMan {
namespace Server {
namespace GameMessagesHandlers {

ReqRespHandler::ReqRespHandler(Server *server) : m_server(server) {}

zmq::message_t ReqRespHandler::handleReqRepl(
    const GameLogic::GameMessages::ConnectionMessageHeader *connHeader) {
  m_logger.logInfo("Handling client request of type '" +
                   toString(connHeader->event) + "'");
  switch (connHeader->event) {
  case GameLogic::GameMessages::ConnectionMessageType::GET_GAMES: {
    auto *clientReq =
        static_cast<const GameLogic::GameMessages::ClientReqGetGames *>(
            (void *)connHeader);
    return handleClientReqRespGetGames(clientReq);
  }
  default:
    return {};
  }
}
zmq::message_t ReqRespHandler::handleClientReqRespGetGames(
    const GameLogic::GameMessages::ClientReqGetGames *clientReq) {
  GameLogic::GameMessages::ClientReplGetGames reply;
  reply.gameId =
      (m_server->m_gameRunner ? m_server->m_gameRunner->getGameId() : -1);

  m_logger.logInfo(
      "Returning result from handleClientReqRespGetGames with Ids: " +
      reply.gameId);

  zmq::message_t replyMsg;
  memcpy(replyMsg.data(), &reply, sizeof(reply));
  return replyMsg;
}

} // namespace GameMessagesHandlers
} // namespace Server
} // namespace PacMan