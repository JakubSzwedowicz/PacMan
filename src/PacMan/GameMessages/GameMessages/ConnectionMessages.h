//
// Created by jakubszwedowicz on 3/23/25.
//

#ifndef CONNECTIONMESSAGES_H
#define CONNECTIONMESSAGES_H

#include <vector>

#include "ConnectionMessageType.h"
#include "MessageHeader.h"

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

struct ConnectionMessageHeader {
  MessageHeader header;
  ConnectionMessageType event;
};

struct ClientReqGetGames {
  ConnectionMessageHeader header = {
      {MessageSource::CLIENT, MessageType::CONNECTION_MESSAGE},
      ConnectionMessageType::GET_GAMES};
};

struct ClientReplGetGames {
  ConnectionMessageHeader header = {
      {MessageSource::SERVER, MessageType::CONNECTION_MESSAGE},
      ConnectionMessageType::GET_GAMES};
  int gameId;
};

} // namespace GameMessages
} // namespace GameLogic
} // namespace PacMan

#endif // CONNECTIONMESSAGES_H
