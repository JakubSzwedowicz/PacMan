//
// Created by Jakub Szwedowicz on 4/12/25.
//

#ifndef BASICGAMEEVENTSPUBLISHERS_H
#define BASICGAMEEVENTSPUBLISHERS_H

#include "GameEventsManager.h"

namespace PacMan {
namespace GameEvents {

class EntityEventsPublisher final
    : public GameEventsManager::EntityEventPublisher_t {};
class GameEventsPublisher final
    : public GameEventsManager::GameEventPublisher_t {};
class GameSessionEventsPublisher final
    : public GameEventsManager::GameSessionEventPublisher_t {};

} // namespace GameEvents
} // namespace PacMan

#endif // BASICGAMEEVENTSPUBLISHERS_H
