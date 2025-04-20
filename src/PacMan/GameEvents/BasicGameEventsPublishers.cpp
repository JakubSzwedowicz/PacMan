//
// Created by Jakub Szwedowicz on 4/12/25.
//

#include "GameEventsManager/BasicGameEventsPublishers.h"

namespace PacMan {
namespace GameEvents {

GameEventsManager getDefaultGameEventsManager() {
  return GameEventsManager{
      std::make_unique<EntityEventsPublisher>(),
      std::make_unique<GameEventsPublisher>(),
      std::make_unique<GameSessionEventsPublisher>(),
  };
}

} // namespace GameEvents
} // namespace PacMan