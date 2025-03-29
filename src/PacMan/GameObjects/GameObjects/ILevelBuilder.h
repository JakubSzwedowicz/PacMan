//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef IBOARDBUILDER_H
#define IBOARDBUILDER_H

#include "Level.h"
#include <memory>

namespace PacMan {
namespace GameObjects {

class ILevelBuilder {
public:
  ILevelBuilder() { reset(); }
  virtual ~ILevelBuilder() = default;
  std::unique_ptr<Level> release() { return std::move(m_level); }

protected:
  void reset() { m_level = nullptr; }
  std::unique_ptr<Level> m_level;
};

} // namespace GameObjects
} // namespace PacMan
#endif // IBOARDBUILDER_H
