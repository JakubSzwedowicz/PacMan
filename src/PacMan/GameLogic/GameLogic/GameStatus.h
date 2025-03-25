//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef GAMESTATUS_H
#define GAMESTATUS_H

#include <cstdint>
#include <string>

namespace PacMan {
namespace GameLogic {

enum class GameStatus : uint8_t { WAITING, RUNNING, PAUSED, FINISHED };

inline std::string toString(const GameStatus &status) {
  switch (status) {
  case GameStatus::WAITING:
    return "WAITING";
  case GameStatus::RUNNING:
    return "RUNNING";
  case GameStatus::PAUSED:
    return "PAUSED";
  case GameStatus::FINISHED:
    return "FINISHED";
  default:
    return "UNKNOWN";
  }
}

inline std::ostream &operator<<(std::ostream &os, const GameStatus &status) {
  return (os << toString(status));
}

} // namespace GameLogic
} // namespace PacMan

#endif // GAMESTATUS_H
