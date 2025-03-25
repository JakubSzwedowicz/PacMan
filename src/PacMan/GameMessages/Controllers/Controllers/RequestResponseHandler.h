//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef REQUESTRESPONSEHANDLER_H
#define REQUESTRESPONSEHANDLER_H

namespace PacMan {
namespace GameMessages {
namespace Controllers {

class RequestResponseHandler {
public:
  RequestResponseHandler(Server *server);
  zmq::message_t handleReqRepl(
      const GameLogic::GameMessages::ConnectionMessageHeader *connHeader);
  zmq::message_t handleClientReqRespGetGames(
      const GameLogic::GameMessages::ClientReqGetGames *clientReq);

private:
  Server *m_server;
  Utils::Logger m_logger =
      Utils::Logger("ReqRespHandler", Utils::LogLevel::INFO);
};

} // namespace Controllers
} // namespace GameMessages
} // namespace PacMan

#endif // REQUESTRESPONSEHANDLER_H
