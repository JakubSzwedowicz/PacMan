//
// Created by jakubszwedowicz on 2/27/25.
//

#ifndef IENTITY_H
#define IENTITY_H

#include "EntityType.h"
#include <cstdint>

namespace PacMan {
namespace GameObjects {
namespace Entities {

template <typename T> struct Position {
  T x;
  T y;

  template <typename TT> explicit Position(const Position<TT> &position);

  bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
  }

  struct Hash {
    size_t operator()(const Position &pos) const {
      return std::hash<T>()(pos.x) ^ std::hash<T>()(pos.y);
    }
  };

  std::string toString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
  }
};

using TilePosition = Position<int>;
using RealPosition = Position<double>;

template <>
template <>
inline TilePosition::Position(const RealPosition &position)
    : x(position.x), y(position.y) {}

template <>
template <>
inline RealPosition::Position(const TilePosition &position)
    : x(position.x), y(position.y) {}

class IEntity {
public:
  IEntity(uint32_t entityId) : m_entityId(entityId) {}
  virtual ~IEntity() = default;

  uint32_t getEntityId() const { return m_entityId; }
  virtual EntityType getEntityType() const = 0;
  friend std::ostream &operator<<(std::ostream &os, const IEntity &entity) {
    return (os << entity.getEntityType());
  }

  TilePosition &getTilePosition() { return m_tilePosition; }
  void setTilePosition(const TilePosition &position) {
    m_tilePosition = position;
  }
  RealPosition &getRealPosition() { return m_realPosition; }
  void setRealPosition(const RealPosition &position) {
    m_realPosition = position;
  }

protected:
  TilePosition m_tilePosition;
  RealPosition m_realPosition;
  uint32_t m_entityId;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // IENTITY_H
