//
// Created by Jakub Szwedowicz on 4/12/25.
//

#ifndef PLAYERSESSIONINFO_H
#define PLAYERSESSIONINFO_H

#include <cstdint>

#include "GameSessionStatus.h"

namespace PacMan::GameLogic {

struct PlayerSessionInfo {
  uint32_t playerUID;
  uint32_t sessionUID;
  GameSessionStatus status;
};
} // namespace PacMan::GameLogic
#endif // PLAYERSESSIONINFO_H
