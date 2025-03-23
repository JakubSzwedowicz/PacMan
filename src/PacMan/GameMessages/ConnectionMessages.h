//
// Created by jakubszwedowicz on 3/23/25.
//

#ifndef CONNECTIONMESSAGES_H
#define CONNECTIONMESSAGES_H

#include "ConnectionMessageType.h"
#include "MessageType.h"

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

struct ConnectionMessageHeader {
  MessageHeader header;
  ConnectionMessageType event;
};

struct ClientReqGetGames {
  ConnectionMessageHeader header = {{MessageSource::CLIENT, MessageType::CONNECTION_MESSAGE}, ConnectionMessageType::GET_GAMES};
};

struct ClientReplGetGames {
  ConnectionMessageHeader header = {{MessageSource::SERVER, MessageType::CONNECTION_MESSAGE}, ConnectionMessageType::GET_GAMES};

};

} // namespace GameMessages
} // namespace GameLogic
} // namespace PacMan

#endif // CONNECTIONMESSAGES_H
