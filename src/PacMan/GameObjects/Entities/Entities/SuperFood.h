//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef SUPERFOOD_H
#define SUPERFOOD_H

#include <cstdint>

#include "Entities/Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class SuperFood : public Entity<EntityType::SUPER_FOOD> {
public:
  SuperFood() : Entity() {}
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // SUPERFOOD_H
