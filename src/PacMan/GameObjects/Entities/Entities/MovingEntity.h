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
  case EntityDirection::NONE:
    return "'NONE'";
  default:
    return "'INVALID!!!'";
  }
}

class MovingEntity : public Entity {
public:
  explicit MovingEntity(const EntityType entityType, const TilePosition startingPosition, Level& level)
      : Entity(entityType), m_startingPosition(startingPosition), m_level(level) {setTilePosition(m_startingPosition);}

  virtual void update(std::chrono::milliseconds deltaTime) = 0;

  void changeDirection(EntityDirection direction) {
    setCurrDirection(direction);
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

  [[nodiscard]] MovementState getMovementState() const {
    return m_movementState;
  }
  void setMovementState(const MovementState movementState) {
    m_movementState = movementState;
  }

  uint32_t decreaseHealth() { return --m_health; }
  uint32_t increaseScore(uint32_t score) { return (m_score += score); }

  [[nodiscard]] uint32_t getScore() const { return m_score; }
  void setScore(int score) { m_score = score; }
  [[nodiscard]] uint32_t getHealth() const { return m_health; }
  void setHealth(int health) { m_health = health; }
  [[nodiscard]] float getSpeedPerSeconds() const { return m_speedInPerSeconds; }
  [[nodiscard]] TilePosition getStartingPosition() const { return m_startingPosition; }
  void setStartingPosition(TilePosition startingPosition) { m_startingPosition = startingPosition; }

protected:
  EntityDirection m_currDirection = EntityDirection::NONE;
  EntityDirection m_nextDirection = EntityDirection::NONE;
  MovementState m_movementState = MovementState::ON_TILE;
  float m_speedInPerSeconds = 0.5f;
  TilePosition m_startingPosition;

  Level& m_level;
  uint32_t m_score = 0;
  uint32_t m_health = 3;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // MOVINGENTITY_H
