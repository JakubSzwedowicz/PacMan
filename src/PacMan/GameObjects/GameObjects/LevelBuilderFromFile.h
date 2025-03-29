//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVELBUILDERFROMFILE_H
#define LEVELBUILDERFROMFILE_H

#include "ILevelBuilder.h"
#include "Utils/Logger.h"

#include <memory>

namespace PacMan {
namespace GameObjects {

class LevelBuilderFromFile : public ILevelBuilder {
public:
  explicit LevelBuilderFromFile(const std::string &boardPath);

private:
  void createLevel();

private:
  mutable Utils::Logger m_logger =
      Utils::Logger("LevelBuilderFromFile", Utils::LogLevel::INFO);
  const std::string &m_boardPath;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVELBUILDERFROMFILE_H
