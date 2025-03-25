//
// Created by jakubszwedowicz on 2/27/25.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>

#include "EntityType.h"
#include "IEntity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class EntityDirection : uint8_t { NONE, UP, DOWN, LEFT, RIGHT };

template <EntityType ENTITY_TYPE> class Entity : public IEntity {
public:
  Entity() : IEntity(s_nextEntityId++) {}
  virtual ~Entity() = default;
  EntityType getEntityType() const override { return m_entityType; }

protected:
  const EntityType m_entityType = ENTITY_TYPE;

private:
  // Overflow is not an issue here
  inline static uint32_t s_nextEntityId = 0;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan
#endif // ENTITY_H
