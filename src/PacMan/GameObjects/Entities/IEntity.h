//
// Created by jakubszwedowicz on 2/27/25.
//

#ifndef IENTITY_H
#define IENTITY_H

#include "EntityType.h"

#include <string>

namespace PacMan {
namespace GameObjects {
namespace Entities {

class IEntity {
public:
  virtual ~IEntity() = default;
  virtual EntityType getEntityType() const = 0;
  friend std::ostream &operator<<(std::ostream &os, const IEntity &entity) {
    return (os << entity.getEntityType());
  }
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // IENTITY_H
