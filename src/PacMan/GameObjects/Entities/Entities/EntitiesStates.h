//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef ENTITIESSTATES_H
#define ENTITIESSTATES_H

#include <string>

namespace PacMan::GameObjects::Entities {

enum class PacManState : uint8_t { NORMAL, EMPOWERED };

enum class GhostState {
  CHASING,
  SCATTERING,
  FRIGHTENED,
  EATEN,
};

inline std::string toString(const GhostState state) {
  switch (state) {
  case GhostState::CHASING:
    return "CHASING";
  case GhostState::SCATTERING:
    return "SCATTERING";
  case GhostState::FRIGHTENED:
    return "FRIGHTENED";
  case GhostState::EATEN:
    return "EATEN";
  }
  return "UNKNOWN " +
         std::to_string(static_cast<std::underlying_type_t<GhostState>>(state));
}

} // namespace PacMan::GameObjects::Entities
#endif // ENTITIESSTATES_H
