//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef PACMAN_H
#define PACMAN_H
#include "Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class PacMan : public Entity<EntityType::PAC_MAN> {};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // PACMAN_H
