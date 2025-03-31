//
// Created by jakubszwedowicz on 3/29/25.
//

#include "GameEventsManager/GameEventsManager.h"

#include "Utils/Logger.h"

namespace PacMan {
namespace GameEvents {

GameEventsManager::GameEventsManager(EntityEventPublisherPtr_t entityEvenPublisher,
                                     GameEventPublisherPtr_t gameEventPublisher)
    : m_logger(std::make_unique<Utils::Logger>("GameEventsManager",
                                               Utils::LogLevel::DEBUG)),
      m_entityEventPublisher(std::move(entityEvenPublisher)), m_gameEventPublisher(std::move(gameEventPublisher)) {}

GameEventsManager::EntityEventPublisher_t & GameEventsManager::getEntityEventPublisher() const {
  return *m_entityEventPublisher;
}

GameEventsManager::GameEventPublisher_t &
GameEventsManager::getGameEventPublisher() const {
  return *m_gameEventPublisher;
}
} // GameEvents
} // PacMan