//
// Created by jakubszwedowicz on 3/23/25.
//

#ifndef CONNECTIONMESSAGES_H
#define CONNECTIONMESSAGES_H

#include <vector>

#include "ConnectionMessageType.h"
#include "MessageType.h"

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

struct ConnectionMessageHeader {
  ConnectionMessageHeader(MessageSource source, MessageType type,
                          ConnectionMessageType connType)
      : header({source, type}), event(connType) {}
  MessageHeader header;
  ConnectionMessageType event;
};

struct ClientReqGetGames : public ConnectionMessageHeader {
  ClientReqGetGames()
      : ConnectionMessageHeader(MessageSource::CLIENT,
                                MessageType::CONNECTION_MESSAGE,
                                ConnectionMessageType::GET_GAMES) {}
};

struct ClientReplGetGames : public ConnectionMessageHeader {
  ClientReplGetGames()
      : ConnectionMessageHeader(MessageSource::SERVER,
                                MessageType::CONNECTION_MESSAGE,
                                ConnectionMessageType::GET_GAMES) {}
  int gameId;
};

} // namespace GameMessages
} // namespace GameLogic
} // namespace PacMan

#endif // CONNECTIONMESSAGES_H
