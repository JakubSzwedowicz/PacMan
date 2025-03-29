//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef PACMAN_H
#define PACMAN_H

#include <cstdint>

#include "EntityType.h"
#include "GameEventsManager/GameEventsManager.h"
#include "MovingEntity.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class PacManState : uint8_t { NORMAL, EMPOWERED };

class PacMan : public MovingEntity,
               public Utils::ISubscriber<GameEvents::EntityEvent> {
public:
  explicit PacMan(Level *level);
  void update(std::chrono::milliseconds deltaTime) override;
  void callback(const GameEvents::EntityEvent &event) override;

  [[nodiscard]] PacManState getPacManState() const { return m_pacManState; }

private:
  void setPacManState(const PacManState pacManState);
  void setNormalPacManState();
  void setEmpoweredPacManState(std::chrono::milliseconds empoweredDuration);

private:
  std::unique_ptr<Utils::ILogger> m_logger;
  GameEvents::GameEventsManager &m_gameEventsManager =
      GameEvents::GameEventsManager::getInstance();
  PacManState m_pacManState = PacManState::NORMAL;
  std::chrono::milliseconds m_empoweredDurationMs =
      std::chrono::milliseconds(0);
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // PACMAN_H
