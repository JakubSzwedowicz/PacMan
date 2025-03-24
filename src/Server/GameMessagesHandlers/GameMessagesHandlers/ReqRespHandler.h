//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef REQRESPHANDLER_H
#define REQRESPHANDLER_H

#include "GameMessages/ConnectionMessages.h"
#include "Utils/Logger.h"

#include <zmq.hpp>

namespace PacMan {
namespace Server {
class Server;

namespace GameMessagesHandlers {

class ReqRespHandler {
public:
  ReqRespHandler(Server *server);
  zmq::message_t handleReqRepl(
      const GameLogic::GameMessages::ConnectionMessageHeader *connHeader);
  zmq::message_t handleClientReqRespGetGames(
      const GameLogic::GameMessages::ClientReqGetGames *clientReq);

private:
  Server *m_server;
  Utils::Logger m_logger =
      Utils::Logger("ReqRespHandler", Utils::LogLevel::INFO);
};

} // namespace GameMessagesHandlers
} // namespace Server
} // namespace PacMan

#endif // REQRESPHANDLER_H
