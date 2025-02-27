//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef FOOD_H
#define FOOD_H

#include "EntityType.h"
#include "IEntity.h"

#include <Entity.h>

namespace PacMan {
namespace GameObjects {
namespace Entities {

class Food : public Entity<EntityType::FOOD> {
public:
private:
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // FOOD_H
