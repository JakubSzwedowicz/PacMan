//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef WALL_H
#define WALL_H
#include "Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class Wall : public Entity<EntityType::WALL> {};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // WALL_H
