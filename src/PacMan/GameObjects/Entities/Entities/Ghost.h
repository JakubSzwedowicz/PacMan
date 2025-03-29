//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef GHOST_H
#define GHOST_H

#include <cstdint>
#include <memory>
#include <vector>

#include "EntitiesStates.h"
#include "EntityType.h"
#include "GameEventsManager/GameEventsManager.h"
#include "MovingEntity.h"

namespace PacMan::Utils {
class ILogger;
}
namespace PacMan {
namespace GameObjects {
namespace Entities {

class Ghost : public MovingEntity,
              public Utils::ISubscriber<GameEvents::EntityEvent> {
public:
  Ghost(Level *level, GameEvents::GameEventsManager& gameEventsManager);
  void update(std::chrono::milliseconds deltaTime) override;
  void callback(const GameEvents::EntityEvent &event) override;

  // Getters/Setters
  [[nodiscard]] GhostState getGhostState() const { return m_ghostState; }

public:
  std::unique_ptr<Utils::ILogger> m_logger;
  Utils::IPublisher<GameEvents::EntityEvent> & m_entityEventsPublisher;
  GhostState m_ghostState = GhostState::CHASING;

  std::chrono::milliseconds m_frightenedDurationMs =
      std::chrono::milliseconds(0);
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // GHOST_H
