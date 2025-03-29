//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef GHOST_H
#define GHOST_H

#include <cstdint>
#include <vector>
#include <memory>

#include "EntityType.h"
#include "MovingEntity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class GhostState {
  CHASING,
  SCATTERING,
  FRIGHTENED,
  EATEN // Transitioning back to pen
};

class Ghost : public MovingEntity {
public:
  Ghost(Level* level) : MovingEntity(EntityType::GHOST, level) {}
  void update(std::chrono::time_point<std::chrono::steady_clock> timePoints) override;

  // Getters/Setters
  [[nodiscard]] GhostState getGhostState() const { return m_ghostState; }

public:
  GhostState m_ghostState = GhostState::CHASING;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // GHOST_H
