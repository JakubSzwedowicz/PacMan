//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEEVENTSMANAGER_H
#define GAMEEVENTSMANAGER_H

#include "GameEvents/GameEvents.h"
#include "Utils/ILogger.h"
#include "Utils/PublishSubscribeHandlers.h"

namespace PacMan {
namespace GameEvents {

class GameEventsManager final : Utils::IPublisher<EntityEvent> {
public:
  explicit GameEventsManager();
  void publish(const EntityEvent &event) override;

public:
  // Accessors
  static GameEventsManager &getInstance();

private:
  std::unique_ptr<Utils::ILogger> m_logger;
};

} // namespace GameEvents
} // namespace PacMan

#endif // GAMEEVENTSMANAGER_H
