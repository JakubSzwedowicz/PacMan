//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef GHOST_H
#define GHOST_H

#include <cstdint>

#include "EntityType.h"
#include "MovingEntity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class GhostBehaviour : uint8_t { CHASE, SCATTER, FRIGHTENED };

class Ghost : public MovingEntity<EntityType::GHOST> {
public:
  Ghost() : MovingEntity() {}
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // GHOST_H
