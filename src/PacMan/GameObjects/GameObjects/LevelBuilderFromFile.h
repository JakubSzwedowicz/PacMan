//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVELBUILDERFROMFILE_H
#define LEVELBUILDERFROMFILE_H

#include <memory>
#include <string>

#include "ILevelBuilder.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace Utils {
class ILogger;
}

namespace GameEvents {
class GameEventsManager;
}

namespace GameObjects {

class LevelBuilderFromFile : public ILevelBuilder {
public:
  explicit LevelBuilderFromFile(const std::string &boardPath, GameEvents::GameEventsManager& gameEventsManager);

private:
  void createLevel();

private:
  mutable std::unique_ptr<Utils::ILogger> m_logger;
  const std::string &m_boardPath;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVELBUILDERFROMFILE_H
