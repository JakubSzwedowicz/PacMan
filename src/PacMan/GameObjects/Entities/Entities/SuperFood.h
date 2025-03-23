//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef SUPERFOOD_H
#define SUPERFOOD_H

#include "Entities/Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class SuperFood : public Entity<EntityType::SUPER_FOOD> {
  SuperFood(uint32_t entityId) : Entity(entityId) {}
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // SUPERFOOD_H
