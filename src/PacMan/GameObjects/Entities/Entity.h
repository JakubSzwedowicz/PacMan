//
// Created by jakubszwedowicz on 2/27/25.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "EntityType.h"
#include "IEntity.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

template <EntityType ENTITY_TYPE> class Entity : public IEntity {
public:
  Entity() = default;
  virtual ~Entity() = default;
  EntityType getEntityType() const override { return m_entityType; }

private:
  const EntityType m_entityType = ENTITY_TYPE;
};
} // namespace Entities
} // namespace GameObjects
} // namespace PacMan
#endif // ENTITY_H
