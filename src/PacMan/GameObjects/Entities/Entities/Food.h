//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef FOOD_H
#define FOOD_H

#include <cstdint>

#include "EntityType.h"

#include "Entities/Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class Food : public Entity {
public:
  Food() : Entity(EntityType::FOOD) {}

private:
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // FOOD_H
