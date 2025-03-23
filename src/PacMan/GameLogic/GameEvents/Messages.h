//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef MESSAGES_H
#define MESSAGES_H

#include <iostream>

#include "GameEvent.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameLogic {
namespace GameEvents {

enum class MessageSource : uint8_t { CLIENT, SERVER };

struct MessageHeader {
  MessageSource source;
  GameEvent event;
};



struct ServerMessageGameCreated {
  MessageHeader header  = {MessageSource::SERVER, GameEvent::GAME_CREATED};
  GameObjects::Level level;
};

struct ServerMessageGameStarted {
  MessageHeader header = {MessageSource::SERVER, GameEvent::GAME_START};
};

struct ServerMessageGameEntityDeath {
  MessageHeader header = {MessageSource::SERVER, GameEvent::ENTITY_DEATH};

};

struct PlayerMessage {
  MessageHeader header;
};

} // namespace GameEvents
} // namespace GameLogic
} // namespace PacMan
#endif // MESSAGES_H
