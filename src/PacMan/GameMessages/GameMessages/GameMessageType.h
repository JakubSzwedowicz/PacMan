//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <cstdint>

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

enum class GameMessageType : uint8_t {
  GAME_CREATED,
  GAME_START,
  GAME_OVER,
  ENTITY_SPAWN,
  ENTITY_DEATH,
  ENTITY_DIRECTION_CHANGE,
  ENTITY_STATE_CHANGE,
  ENTITY_COLLISION,
  FOOD_EATEN
};

}

} // namespace GameLogic
} // namespace PacMan

#endif // GAMEEVENT_H
