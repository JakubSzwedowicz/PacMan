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
  PELLET = '.',
  SUPER_PELLET = 'S',
  EMPTY = ' '
};

inline std::string toString(EntityType entityType) {
  switch (entityType) {
  case EntityType::BRIDGE:
    return "BRIDGE";
  case EntityType::EMPTY:
    return "EMPTY";
  case EntityType::PELLET:
    return "PELLET";
  case EntityType::GHOST:
    return "GHOST";
  case EntityType::PAC_MAN:
    return "PAC_MAN";
  case EntityType::SUPER_PELLET:
    return "SUPER_PELLET";
  case EntityType::WALL:
    return "WALL";
  default:
    return "UNKNOWN";
  }
}

inline std::ostream &operator<<(std::ostream &os,
                                const EntityType &entityType) {
  return (os << toString(entityType));
}

enum class GhostType {
  BLINKY,
  CLYDE,
  INKY,
  PINKY,
};

inline std::string toString(GhostType ghostType) {
  switch (ghostType) {
  case GhostType::BLINKY:
    return "BLINKY";
  case GhostType::CLYDE:
    return "CLYDE";
  case GhostType::INKY:
    return "INKY";
  case GhostType::PINKY:
    return "PINKY";
  default:
    return "UNKNOWN";
  }
}

inline std::ostream &operator<<(std::ostream &os, const GhostType &ghostType) {
  return (os << toString(ghostType));
}

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // ENTITYUID_H
