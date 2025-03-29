//
// Created by Jakub Szwedowicz on 2/24/25.
//

#include "Entities/Ghost.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

Ghost::Ghost(Level *level, GameEvents::GameEventsManager &gameEventsManager)
    : MovingEntity(EntityType::GHOST, level),
      ISubscriber(&gameEventsManager.getEntityEventPublisher()), m_entityEventsPublisher(gameEventsManager.getEntityEventPublisher()) {}

void Ghost::update(std::chrono::milliseconds deltaTime) {}

void Ghost::callback(const GameEvents::EntityEvent &event) {}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan