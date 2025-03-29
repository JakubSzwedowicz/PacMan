//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include <chrono>

#include "Entity.h"

namespace PacMan {
namespace GameObjects {
// Forward declare it!
class Level;

namespace Entities {

enum class MovementState : uint8_t { ON_TILE, IN_TRANSIT };
enum class EntityDirection : uint8_t { NONE, UP, DOWN, LEFT, RIGHT };
inline std::string toString(const EntityDirection &direction) {
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
    return "'INVALID!!!'";
  }
}

class MovingEntity : public Entity {
public:
  explicit MovingEntity(const EntityType entityType, Level* level) : Entity(entityType), m_level(level) {}

  virtual void update(std::chrono::time_point<std::chrono::steady_clock> timePoints) = 0;

  void changeDirection() {
    setCurrDirection(getNextDirection());
    setNextDirection(EntityDirection::NONE);
    setMovementState(MovementState::IN_TRANSIT);
  }

  void setTilePosition(const TilePosition &tilePosition) override {
    m_movementState = MovementState::ON_TILE;
    Entity::setTilePosition(tilePosition);
  }

  [[nodiscard]] EntityDirection getCurrDirection() const {
    return m_currDirection;
  }
  void setCurrDirection(const EntityDirection direction) {
    m_currDirection = direction;
  }

  [[nodiscard]] EntityDirection getNextDirection() const {
    return m_nextDirection;
  }
  void setNextDirection(const EntityDirection direction) {
    m_nextDirection = direction;
  }

  [[nodiscard]] EntityState getEntityState() const { return m_entityState; }
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
  float getSpeedPerSeconds() const { return m_speedInPerSeconds; }

protected:
  EntityDirection m_currDirection = EntityDirection::NONE;
  EntityDirection m_nextDirection = EntityDirection::NONE;
  MovementState m_movementState = MovementState::ON_TILE;
  float m_speedInPerSeconds = 0.5f;

  Level* m_level;
  int m_score = 0;
  int m_health = 3;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // MOVINGENTITY_H
