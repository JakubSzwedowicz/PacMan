//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef MESSAGES_H
#define MESSAGES_H

#include <cstdint>

#include "../../GameObjects/GameObjects/Level.h"
#include "GameMessageType.h"
#include "MessageType.h"

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

struct GameMessageHeader {
  MessageHeader source;
  GameMessageType event;
};

struct ServerMessageGameCreated {
  GameMessageHeader header = {
      {MessageSource::SERVER, MessageType::GAME_MESSAGE},
      GameMessageType::GAME_CREATED};
  GameObjects::Level level;
};

struct ServerMessageGameStarted {
  GameMessageHeader header = {
      {MessageSource::SERVER, MessageType::GAME_MESSAGE},
      GameMessageType::GAME_START};
};

struct ServerMessageGameEntityDeath {
  GameMessageHeader header = {
      {MessageSource::SERVER, MessageType::GAME_MESSAGE},
      GameMessageType::ENTITY_DEATH};
};

struct PlayerMessage {
  GameMessageHeader header;
};

} // namespace GameMessages
} // namespace GameLogic
} // namespace PacMan
#endif // MESSAGES_H
