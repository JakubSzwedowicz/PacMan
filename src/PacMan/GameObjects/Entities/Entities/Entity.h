//
// Created by jakubszwedowicz on 2/27/25.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>

#include "EntityType.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

template <typename T> struct Position {
  T x;
  T y;

  explicit Position() : x(0), y(0) {}
  template <typename TT> Position(TT x, TT y) : x(x), y(y) {}
  template <typename TT> explicit Position(const Position<TT> &position) : x(position.x), y(position.y) {};

  bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Position &other) const {
    return x != other.x || y != other.y;
  }

  Position operator+(const Position &other) const {
    return Position(x + other.x, y + other.y);
  }

  Position operator-(const Position &other) const {
    return Position(x - other.x, y - other.y);
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
using EntityId = uint32_t;

enum class EntityState : uint8_t {
  ALIVE,
  DEAD,
};

class Entity {
public:
  Entity(EntityType entityType)
      : m_entityId(s_nextEntityId++), m_entityType(entityType) {}
  Entity(EntityId entityId, EntityType entityType)
      : m_entityId(entityId), m_entityType(entityType) {}
  virtual ~Entity() = default;

  [[nodiscard]] EntityId getEntityId() const { return m_entityId; }
  [[nodiscard]] EntityType getEntityType() const { return m_entityType; }

  [[nodiscard]] std::string toString() const { return "{EntityId: " + std::to_string(m_entityId) + ", EntityType: '" + Entities::toString(m_entityType) + "'}";  }
  friend std::ostream &operator<<(std::ostream &os, const Entity &entity) {
    return (os << entity.getEntityType());
  }

  TilePosition &getMutableTilePosition() { return m_tilePosition; }
  const TilePosition &getTilePosition() const { return m_tilePosition; }
  virtual void setTilePosition(const TilePosition &position) {
    m_tilePosition = position;
    // Compensate for time and float errors:
    setRealPosition(RealPosition(position));
  }

  RealPosition &getMutableRealPosition() { return m_realPosition; }
  [[nodiscard]] const RealPosition &getRealPosition() const { return m_realPosition; }
  void setRealPosition(const RealPosition &position) {
    m_realPosition = position;
  }

protected:
  EntityId m_entityId;
  EntityType m_entityType;
  EntityState m_entityState;
  TilePosition m_tilePosition = {0, 0};
  RealPosition m_realPosition = {0, 0};

  // Overflow is not an issue here
  inline static uint32_t s_nextEntityId = 0;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan
#endif // ENTITY_H
