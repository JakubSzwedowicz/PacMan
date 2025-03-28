//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class MovementState : uint8_t { ON_TILE, IN_TRANSIT };
enum class EntityDirection : uint8_t { NONE, UP, DOWN, LEFT, RIGHT };
std::string toString(const EntityDirection& direction) {
  switch (direction) {
    case EntityDirection::UP:
      return "'UP'";
    case EntityDirection::DOWN:
      return "'DOWN'";
    case EntityDirection::LEFT:
      return "'LEFT'";
    case EntityDirection::RIGHT:
      return "'RIGHT'";
    default:
      return "''";
  }
}

class MovingEntity : public Entity {
public:
  explicit MovingEntity(const EntityType entityType) : Entity(entityType) {}

  EntityDirection getCurrDirection() const { return m_currDirection; }
  void setCurrDirection(const EntityDirection direction) {
    m_currDirection = direction;
  }
  EntityDirection getNextDirection() const { return m_nextDirection; }
  void setNextDirection(const EntityDirection direction) {
    m_nextDirection = direction;
  }

  EntityState getEntityState() const { return m_entityState; }
  void setEntityState(const EntityState entityState) {
    m_entityState = entityState;
  }

  MovementState getMovementState() const { return m_movementState; }
  void setMovementState(const MovementState movementState) {
    m_movementState = movementState;
  }

  int getScore() const { return m_score; }
  void setScore(int score) { m_score = score; }
  int getHealth() const { return m_health; }
  void setHealth(int health) { m_health = health; }
  float getSpeedIn100Ms() const { return m_speedIn100Ms; }

protected:
  EntityDirection m_currDirection = EntityDirection::NONE;
  EntityDirection m_nextDirection = EntityDirection::NONE;
  MovementState m_movementState = MovementState::ON_TILE;
  float m_speedIn100Ms = 0.1f;

  int m_score = 0;
  int m_health = 3;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // MOVINGENTITY_H
