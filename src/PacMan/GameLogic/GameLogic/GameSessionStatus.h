//
// Created by Jakub Szwedowicz on 4/12/25.
//

#ifndef GAMESESSIONSTATUS_H
#define GAMESESSIONSTATUS_H

#include <cstdint>

namespace PacMan::GameLogic {

enum class GameSessionStatus : uint8_t {
  WAITING,
  PAUSED,
  FINISHED,
};

}
#endif // GAMESESSIONSTATUS_H
