//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef EMPTY_H
#define EMPTY_H

#include "Entity.h"
#include "EntityType.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class Empty : public Entity<EntityType::EMPTY> {};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // EMPTY_H
