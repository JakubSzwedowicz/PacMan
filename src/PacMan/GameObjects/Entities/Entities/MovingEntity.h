//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "Entity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class EntityState : uint8_t {
  ALIVE,
  DEAD,
};

enum class MovementState : uint8_t { ON_TILE, IN_TRANSIT };

template <EntityType ENTITY_TYPE>
class MovingEntity : public Entity<ENTITY_TYPE> {
public:
  MovingEntity() : Entity<ENTITY_TYPE>() {}

  EntityDirection getCurrDirection() const { return m_currDirection; }
  void setCurrDirection(EntityDirection direction) {
    m_currDirection = direction;
  }
  EntityDirection getNextDirection() const { return m_nextDirection; }
  void setNextDirection(EntityDirection direction) {
    m_nextDirection = direction;
  }

  EntityState getEntityState() const { return m_entityState; }
  void setEntityState(EntityState entityState) { m_entityState = entityState; }

  MovementState getMovementState() const { return m_movementState; }
  void setMovementState(MovementState movementState) {
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
  EntityState m_entityState = EntityState::DEAD;
  MovementState m_movementState = MovementState::ON_TILE;
  float m_speedIn100Ms = 0.1f;
  int m_score = 0;
  int m_health = 3;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // MOVINGENTITY_H
