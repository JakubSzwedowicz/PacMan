//
// Created by jakubszwedowicz on 3/31/25.
//

#ifndef BASEEVENTS_H
#define BASEEVENTS_H

#include <chrono>

//=============================================================================
// Base Event Structure
//=============================================================================
namespace PacMan::GameEvents {
/**
 * @brief Base struct for all game events.
 * Provides a common type and virtual destructor for safe polymorphism.
 */
struct BaseEvent {
  std::chrono::time_point<std::chrono::steady_clock> timestamp =
      std::chrono::steady_clock::now();
  virtual ~BaseEvent() = default;
};

}

#endif // BASEEVENTS_H
