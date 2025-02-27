//
// Created by Jakub Szwedowicz on 2/24/25.
//

#ifndef GHOST_H
#define GHOST_H

#include <Entity.h>

namespace PacMan {
namespace GameObjects {
namespace Entities {

class Ghost : public Entity<EntityType::GHOST> {};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // GHOST_H
