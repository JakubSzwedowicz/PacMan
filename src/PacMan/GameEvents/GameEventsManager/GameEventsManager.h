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
  explicit GameEventsManager(
      std::unique_ptr<Utils::IPublisher<EntityEvent>> entityEvenPublisher);
  ~GameEventsManager() = default;
  [[nodiscard]] Utils::IPublisher<EntityEvent> &getEntityEventPublisher() const;

private:
  std::unique_ptr<Utils::ILogger> m_logger;
  std::unique_ptr<Utils::IPublisher<EntityEvent>> m_entityEventPublisher;
};

} // namespace GameEvents
} // namespace PacMan

#endif // GAMEEVENTSMANAGER_H
