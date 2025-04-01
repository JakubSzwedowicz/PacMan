//
// Created by jakubszwedowicz on 3/31/25.
//

#ifndef GAMESTATUS_H
#define GAMESTATUS_H

#include <iostream>
#include <string>
#include <cstdint>

namespace PacMan::GameLogic {

enum class GameStatus : uint8_t {
  CREATING,
  WAITING,
  RUNNING,
  PAUSED,
  FINISHED
};

inline std::string toString(const GameStatus &status) {
  switch (status) {
  case GameStatus::CREATING:
    return "CREATING";
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

} // namespace PacMan::GameLogic
#endif // GAMESTATUS_H
