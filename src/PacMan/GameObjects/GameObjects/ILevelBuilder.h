//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef IBOARDBUILDER_H
#define IBOARDBUILDER_H

#include <memory>

#include "GameEventsManager/GameEventsManager.h"
#include "Level.h"

namespace PacMan {
namespace GameObjects {

class ILevelBuilder {
public:
  ILevelBuilder(GameEvents::GameEventsManager& gameEventsManager) : m_gameEventsManager(gameEventsManager) { reset(); }
  virtual ~ILevelBuilder() = default;
  std::unique_ptr<Level> release() { return std::move(m_level); }

protected:
  void reset() { m_level = nullptr; }
  std::unique_ptr<Level> m_level;
  GameEvents::GameEventsManager& m_gameEventsManager;
};

} // namespace GameObjects
} // namespace PacMan
#endif // IBOARDBUILDER_H
