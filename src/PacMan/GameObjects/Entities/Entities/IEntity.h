//
// Created by jakubszwedowicz on 2/27/25.
//

#ifndef IENTITY_H
#define IENTITY_H

#include "EntityType.h"

namespace PacMan {
namespace GameObjects {
namespace Entities {

class IEntity {
public:
  IEntity(uint32_t entityId) : m_entityId(entityId) {}
  virtual ~IEntity() = default;

  uint32_t getEntityId() const { return m_entityId; }
  virtual EntityType getEntityType() const = 0;
  friend std::ostream &operator<<(std::ostream &os, const IEntity &entity) {
    return (os << entity.getEntityType());
  }
protected:
  uint32_t m_entityId;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // IENTITY_H
