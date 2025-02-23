//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef ENTITYUID_H
#define ENTITYUID_H

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class EntityType : char {
  PAC_MAN = 'P',
  GHOST = 'G',
  WALL = '#',
  BRIDGE = 'B',
  FOOD = 'F',
  SUPER_FOOD = 'S',
  EMPTY = ' '
};

}
} // namespace GameObjects
} // namespace PacMan

#endif // ENTITYUID_H
