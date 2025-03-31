//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef PACMAN_H
#define PACMAN_H

#include <cstdint>

#include "EntitiesStates.h"
#include "EntityType.h"
#include "GameEventsManager/GameEventsManager.h"
#include "MovingEntity.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class PacMan : public MovingEntity,
               public Utils::ISubscriber<GameEvents::EntityEvent> {
public:
  explicit PacMan(TilePosition startingPosition, Level& level, GameEvents::GameEventsManager& gameEventsManager);
  void update(std::chrono::milliseconds deltaTime) override;
  void callback(const GameEvents::EntityEvent &event) override;

  [[nodiscard]] PacManState getPacManState() const { return m_pacManState; }

private:
  uint32_t increaseScore(uint32_t score) override;
  void setPacManState(const PacManState pacManState);
  void setNormalPacManState();
  void setEmpoweredPacManState(std::chrono::milliseconds empoweredDuration);

private:
  std::unique_ptr<Utils::ILogger> m_logger;
  Utils::IPublisher<GameEvents::EntityEvent>& m_entityEventsPublisher;
  PacManState m_pacManState = PacManState::NORMAL;
  std::chrono::milliseconds m_empoweredDurationMs =
      std::chrono::milliseconds(0);
};

struct PacManBuilder {
  TilePosition startingPosition = {0, 0};
  Level* level = nullptr;
  GameEvents::GameEventsManager& gameEventsManager;

  std::unique_ptr<PacMan> build();
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // PACMAN_H
