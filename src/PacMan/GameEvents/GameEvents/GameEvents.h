//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include <chrono>
#include <vector>

#include "BaseEvents.h"
#include "GameLogic/GameStatus.h"

namespace PacMan::GameEvents {
using namespace std::chrono_literals;
//=============================================================================
// Gameplay State Events (Inherit directly from BaseEvent)
//=============================================================================

/**
 * @brief Event published when the game starts for the first time or after a
 * reset.
 */
enum class GameEventType : uint8_t { GAME_STATUS_CHANGED };

struct GameEvent : public BaseEvent {
  int gameRunnerId;
  GameEventType gameEventType;
  GameEvent(int gameRunnerId, GameEventType gameEventType)
      : gameRunnerId(gameRunnerId), gameEventType(gameEventType) {}
};

/**
 * @brief Event published when a new level begins.
 */
struct GameStatusChanged final : public GameEvent {
  GameLogic::GameStatus gameStatus;

  explicit GameStatusChanged(int gameRunnerId, GameLogic::GameStatus gameStatus)
      : GameEvent(gameRunnerId, GameEventType::GAME_STATUS_CHANGED),
        gameStatus(gameStatus) {}
};

} // namespace PacMan::GameEvents

#endif // GAMEEVENTS_H
