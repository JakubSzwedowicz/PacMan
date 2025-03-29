//
// Created by jakubszwedowicz on 3/29/25.
//

#include "GameEventsManager/GameEventsManager.h"

#include "Utils/Logger.h"

namespace PacMan {
namespace GameEvents {

GameEventsManager::GameEventsManager(
    std::unique_ptr<Utils::IPublisher<EntityEvent>> entityEventPublisher)
    : m_logger(std::make_unique<Utils::Logger>("GameEventsManager",
                                               Utils::LogLevel::DEBUG)),
      m_entityEventPublisher(std::move(entityEventPublisher)) {}

Utils::IPublisher<EntityEvent>& GameEventsManager::getEntityEventPublisher() const {
  return *m_entityEventPublisher;
}
} // GameEvents
} // PacMan