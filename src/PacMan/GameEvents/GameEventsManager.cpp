//
// Created by jakubszwedowicz on 3/29/25.
//

#include "GameEventsManager/GameEventsManager.h"

#include "Utils/Logger.h"

namespace PacMan {
namespace GameEvents {

GameEventsManager::GameEventsManager(
    EntityEventPublisherPtr_t entityEvenPublisherPtr,
    GameEventPublisherPtr_t gameEventPublisherPtr,
    GameSessionEventPublisherPtr_t gameSessionEventPublisherPtr)
    : m_logger(std::make_unique<Utils::Logger>("GameEventsManager",
                                               Utils::LogLevel::DEBUG)),
      m_entityEventPublisherPtr(std::move(entityEvenPublisherPtr)),
      m_gameEventPublisherPtr(std::move(gameEventPublisherPtr)),
      m_gameSessionEventPublisherPtr(std::move(gameSessionEventPublisherPtr)) {}

GameEventsManager::EntityEventPublisher_t &
GameEventsManager::getEntityEventPublisher() const {
  return *m_entityEventPublisherPtr;
}

GameEventsManager::GameEventPublisher_t &
GameEventsManager::getGameEventPublisher() const {
  return *m_gameEventPublisherPtr;
}

GameEventsManager::GameSessionEventPublisher_t &
GameEventsManager::getGameSessionEventPublisher() const {
  return *m_gameSessionEventPublisherPtr;
}

} // namespace GameEvents
} // namespace PacMan