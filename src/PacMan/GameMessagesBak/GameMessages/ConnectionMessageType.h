//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef CONNECTIONEVENT_H
#define CONNECTIONEVENT_H

#include <cstdint>
#include <sstream>

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

enum class ConnectionMessageType : uint8_t {
  GET_GAMES,
  CONNECT_TO_GAME,
};

inline std::string toString(const ConnectionMessageType &type) {
  switch (type) {
  case ConnectionMessageType::GET_GAMES:
    return "GET_GAMES";
  case ConnectionMessageType::CONNECT_TO_GAME:
    return "CONNECT_TO_GAME";
  default:
    return "UNKNOWN";
  }
}

inline std::ostream &operator<<(std::ostream &os,
                                const ConnectionMessageType &type) {
  return (os << toString(type));
}

} // namespace proto
} // namespace GameLogic
} // namespace PacMan

#endif // CONNECTIONEVENT_H
