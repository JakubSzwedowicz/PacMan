//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef GHOST_H
#define GHOST_H

#include <cstdint>
#include <memory>
#include <vector>

#include "EntityType.h"
#include "GameEventsManager/GameEventsManager.h"
#include "MovingEntity.h"

namespace PacMan::Utils {
class ILogger;
}
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
  Ghost(Level *level) : MovingEntity(EntityType::GHOST, level) {}
  void update(std::chrono::milliseconds deltaTime) override;

  // Getters/Setters
  [[nodiscard]] GhostState getGhostState() const { return m_ghostState; }

public:
  std::unique_ptr<Utils::ILogger> m_logger;
  GameEvents::GameEventsManager &m_gameEventsManager =
      GameEvents::GameEventsManager::getInstance();
  GhostState m_ghostState = GhostState::CHASING;

  std::chrono::milliseconds m_frightenedDurationMs =
      std::chrono::milliseconds(0);
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // GHOST_H
