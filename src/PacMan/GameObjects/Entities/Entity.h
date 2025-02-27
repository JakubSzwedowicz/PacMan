//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef ENTITY_H
#define ENTITY_H
#include "EntityType.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

template <EntityType ENTITY_TYPE> class Entity {
public:
  Entity();
  virtual ~Entity() = default;

  EntityType getEntityType() const { return m_entityType; }

private:
  const EntityType m_entityType = ENTITY_TYPE;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // ENTITY_H
