//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEEVENTSMANAGER_H
#define GAMEEVENTSMANAGER_H

#include <memory>

#include "GameEvents/EntityEvents.h"
#include "GameEvents/GameEvents.h"
#include "GameEvents/GameSessionEvents.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"

namespace PacMan {
namespace GameEvents {

class GameEventsManager {
public:
  using EntityEventPublisher_t = Utils::IPublisher<EntityEvent>;
  using GameEventPublisher_t = Utils::IPublisher<GameEvent>;
  using GameSessionEventPublisher_t = Utils::IPublisher<GameSessionEvent>;

  using EntityEventPublisherPtr_t = std::unique_ptr<EntityEventPublisher_t>;
  using GameEventPublisherPtr_t = std::unique_ptr<GameEventPublisher_t>;
  using GameSessionEventPublisherPtr_t = std::unique_ptr<GameSessionEventPublisher_t>;

  explicit GameEventsManager(EntityEventPublisherPtr_t entityEvenPublisherPtr,
                             GameEventPublisherPtr_t gameEventPublisherPtr,
                             GameSessionEventPublisherPtr_t gameSessionEventPublisherPtr);
  ~GameEventsManager() = default;
  [[nodiscard]] EntityEventPublisher_t &getEntityEventPublisher() const;
  [[nodiscard]] GameEventPublisher_t &getGameEventPublisher() const;
  [[nodiscard]] GameSessionEventPublisher_t& getGameSessionEventPublisher() const;

private:
  std::unique_ptr<Utils::ILogger> m_logger;
  EntityEventPublisherPtr_t m_entityEventPublisherPtr;
  GameEventPublisherPtr_t m_gameEventPublisherPtr;
  GameSessionEventPublisherPtr_t m_gameSessionEventPublisherPtr;
};

} // namespace GameEvents
} // namespace PacMan

#endif // GAMEEVENTSMANAGER_H
