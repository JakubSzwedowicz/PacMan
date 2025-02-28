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
  bool release(std::unique_ptr<Level> &level) {
    if (level == nullptr) {
      return false;
    }
    m_level = std::move(level);
    return (level == nullptr);
  }

protected:
  void reset() { m_level = std::make_unique<Level>(); }

  virtual void setBoard(std::unique_ptr<Level::Board_t> board) {
    m_level->setBoard(std::move(board));
  };

private:
  std::unique_ptr<Level> m_level;
};

} // namespace GameObjects
} // namespace PacMan
#endif // IBOARDBUILDER_H
