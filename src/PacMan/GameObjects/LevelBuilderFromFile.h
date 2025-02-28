//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVELBUILDERFROMFILE_H
#define LEVELBUILDERFROMFILE_H

#include "ILevelBuilder.h"
#include <memory>

namespace PacMan {
namespace GameObjects {

class LevelBuilderFromFile : public ILevelBuilder {
public:
  LevelBuilderFromFile(const std::string &boardPath);

private:
  std::unique_ptr<Level::Board_t> createBoard() const;
  const std::string &m_boardPath;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVELBUILDERFROMFILE_H
