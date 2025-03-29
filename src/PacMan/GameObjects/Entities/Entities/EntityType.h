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

inline std::string toString(EntityType entityType) {
  switch (entityType) {
  case EntityType::BRIDGE:
    return "B";
  case EntityType::EMPTY:
    return " ";
  case EntityType::FOOD:
    return ".";
  case EntityType::GHOST:
    return "G";
  case EntityType::PAC_MAN:
    return "P";
  case EntityType::SUPER_FOOD:
    return "S";
  case EntityType::WALL:
    return "#";
  default:
    return "?";
  }
}

inline std::ostream &operator<<(std::ostream &os,
                                const EntityType &entityType) {
  return (os << toString(entityType));
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // ENTITYUID_H
