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
#include "Strategies/GhostStrategies.h"

#include <unordered_map>

namespace PacMan::Utils {
class ILogger;
}
namespace PacMan {
namespace GameObjects {
namespace Entities {

using GhostStateToGhostStrategies_t = std::unordered_map<GhostState, std::unique_ptr<GameLogic::Strategies::IGhostStrategy>>;
using GhostTypeToGhostStateToGhostStrategies_t = std::unordered_map<GhostType, GhostStateToGhostStrategies_t>;

class Ghost : public MovingEntity,
              public Utils::ISubscriber<GameEvents::EntityEvent> {
public:
  Ghost(GhostType ghostType, TilePosition startingPosition, Level& level,
        GameEvents::GameEventsManager &gameEventsManager);
  void update(std::chrono::milliseconds deltaTime) override;
  void callback(const GameEvents::EntityEvent &event) override;

  // Getters/Setters
  [[nodiscard]] GhostType getGhostType() const { return m_ghostType; }
  [[nodiscard]] GhostState getGhostState() const { return m_ghostState; }
  [[nodiscard]] TilePosition getScatteringPosition() const;
  void setScatteringPosition(TilePosition scatteringPosition);
  void setGhostStrategies(const GhostStateToGhostStrategies_t& ghostStrategies);
  std::string toString() const;

private:
  void setGhostState(GhostState newState);
  void setFrightenedState(std::chrono::milliseconds duration);
  void setDeadState();
  void revertFromFrightenedState();

  void reverseDirection();
  EntityDirection getDirectionBasedOnAdjacentTiles(TilePosition start, TilePosition adjacent) const;
  EntityDirection getDirectionAtJunction(TilePosition junctionPosition) const;

private:
  std::unique_ptr<Utils::ILogger> m_logger;
  GhostType m_ghostType;
  TilePosition m_scatterPosition;
  Utils::IPublisher<GameEvents::EntityEvent> &m_entityEventsPublisher;
  GhostState m_ghostState = GhostState::SCATTERING;
  GhostState m_previousGhostState = GhostState::SCATTERING;
  GhostStateToGhostStrategies_t m_ghostStrategies;

  std::chrono::milliseconds m_frightenedDurationMs =
      std::chrono::milliseconds(0);
};

struct GhostBuilder {
  GhostType ghostType = GhostType::BLINKY;
  TilePosition startingPosition = {0, 0};
  Level *level = nullptr;
  GameEvents::GameEventsManager &gameEventsManager;

  std::unique_ptr<Ghost> build();
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // GHOST_H
