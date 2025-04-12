//
// Created by Jakub Szwedowicz on 4/12/25.
//

#ifndef CLIENTAPPEVENTS_H
#define CLIENTAPPEVENTS_H

#include "BaseEvents.h"

namespace PacMan::GameEvents {

enum class ClientUIEventType : uint8_t { CREATE_SINGLE_PLAYER_GAME_EVENT };

struct ClientUIEvent : public BaseEvent {
  const std::string GameEvent(int gameRunnerId, GameEventType gameEventType)
      : gameRunnerId(gameRunnerId), gameEventType(gameEventType) {}
};

struct ClientAppEvent

} // namespace PacMan::GameEvents
#endif // CLIENTAPPEVENTS_H
