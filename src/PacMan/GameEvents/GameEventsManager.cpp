//
// Created by jakubszwedowicz on 3/29/25.
//

#include "GameEventsManager/GameEventsManager.h"

#include "Utils/Logger.h"

namespace PacMan {
namespace GameEvents {

GameEventsManager::GameEventsManager()
    : m_logger(std::make_unique<Utils::Logger>("GameEventsManager",
                                               Utils::LogLevel::DEBUG)) {}

void GameEventsManager::publish(const EntityEvent &event) {
  m_logger->logDebug("Publishing event " + event.toString());
  IPublisher<EntityEvent>::publish(event);
}

GameEventsManager &GameEventsManager::getInstance() {
  static GameEventsManager instance;
  return instance;
}
} // GameEvents
} // PacMan