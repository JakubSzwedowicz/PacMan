//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEEVENTSMANAGER_H
#define GAMEEVENTSMANAGER_H

#include <memory>

#include "GameEvents/GameEvents.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"

namespace PacMan {
namespace GameEvents {

class GameEventsManager {
public:
  using EntityEventPublisher_t = Utils::IPublisher<EntityEvent>;
  using GameEventPublisher_t = Utils::IPublisher<GameEvent>;
  using EntityEventPublisherPtr_t = std::unique_ptr<EntityEventPublisher_t>;
  using GameEventPublisherPtr_t = std::unique_ptr<GameEventPublisher_t>;
  explicit GameEventsManager(EntityEventPublisherPtr_t entityEvenPublisher,
                             GameEventPublisherPtr_t gameEventPublisher);
  ~GameEventsManager() = default;
  [[nodiscard]] EntityEventPublisher_t &getEntityEventPublisher() const;
  [[nodiscard]] GameEventPublisher_t &getGameEventPublisher() const;

private:
  std::unique_ptr<Utils::ILogger> m_logger;
  EntityEventPublisherPtr_t m_entityEventPublisher;
  GameEventPublisherPtr_t m_gameEventPublisher;
};

} // namespace GameEvents
} // namespace PacMan

#endif // GAMEEVENTSMANAGER_H
