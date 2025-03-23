//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef CONNECTIONEVENT_H
#define CONNECTIONEVENT_H

#include <cstdint>

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

enum class ConnectionMessageType : uint8_t {
  GET_GAMES,
  CONNECT_TO_GAME,
};

}
} // namespace GameLogic
} // namespace PacMan

#endif // CONNECTIONEVENT_H
