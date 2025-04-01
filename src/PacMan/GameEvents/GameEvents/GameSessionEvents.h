//
// Created by jakubszwedowicz on 4/1/25.
//

#ifndef GAMESESSIONEVENTS_H
#define GAMESESSIONEVENTS_H

#include <string>

#include "BaseEvents.h"
#include "Entities/Entity.h"
#include "GameLogic/GameStatus.h"

namespace PacMan::GameEvents {
enum class GameSessionEventType : uint8_t {
  CREATE_SESSION,
  SESSION_INFO,
  PLAYER_UPDATE,
  DESTROY_SESSION,
};

enum class PlayerUpdate : uint8_t { PLAYER_JOINED, PLAYER_DISCONNECTED };

struct SessionInfo {
  uint32_t playersNumber;
  const std::string &boardName;
  GameLogic::GameStatus status;
};

struct GameSessionEvent : public BaseEvent {
  int gameSessionId;
  GameSessionEventType gameSessionEventType;
  GameSessionEvent(int gameSessionId, GameSessionEventType gameSessionEventType)
      : gameSessionId(gameSessionId),
        gameSessionEventType(gameSessionEventType) {}
};

struct CreateSessionEvent : public GameSessionEvent {
  const SessionInfo &sessionInfo;
  CreateSessionEvent(int gameSessionId,
                     const SessionInfo &sessionInfo)
      : GameSessionEvent(gameSessionId, GameSessionEventType::CREATE_SESSION),
        sessionInfo(sessionInfo) {}
};

struct SessionInfoEvent : public GameSessionEvent {
  const SessionInfo &sessionInfo;
  SessionInfoEvent(int gameSessionId, const SessionInfo &sessionInfo)
      : GameSessionEvent(gameSessionId, GameSessionEventType::SESSION_INFO),
        sessionInfo(sessionInfo) {}
};

struct PlayerUpdateEvent : public GameSessionEvent {
  GameObjects::Entities::EntityId playerId;
  PlayerUpdate playerUpdate;
  PlayerUpdateEvent(int gameSessionId, GameObjects::Entities::EntityId playerId,
               PlayerUpdate playerUpdate)
      : GameSessionEvent(gameSessionId, GameSessionEventType::PLAYER_UPDATE),
        playerId(playerId), playerUpdate(playerUpdate) {}
};

struct DestroySessionEvent : public GameSessionEvent {
  DestroySessionEvent(int gameSessionId)
      : GameSessionEvent(gameSessionId, GameSessionEventType::DESTROY_SESSION) {}
};
}
#endif // GAMESESSIONEVENTS_H
