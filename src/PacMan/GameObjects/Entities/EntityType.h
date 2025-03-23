//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef ENTITYUID_H
#define ENTITYUID_H
#include <ostream>

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class EntityType : char {
  PAC_MAN = 'P',
  GHOST = 'G',
  WALL = '#',
  BRIDGE = 'B',
  FOOD = '.',
  SUPER_FOOD = 'S',
  EMPTY = ' '
};

inline std::ostream &operator<<(std::ostream &os,
                                const EntityType &entityType) {
  switch (entityType) {
  case EntityType::PAC_MAN:
    return (os << 'P');
  case EntityType::GHOST:
    return (os << 'G');
  case EntityType::WALL:
    return (os << '#');
  case EntityType::BRIDGE:
    return (os << 'B');
  case EntityType::FOOD:
    return (os << '.');
  case EntityType::SUPER_FOOD:
    return (os << 'S');
  case EntityType::EMPTY:
    return (os << ' ');
  default:
    return (os << '?');
  }
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // ENTITYUID_H
