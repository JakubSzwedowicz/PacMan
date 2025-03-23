#include <csignal>
#include <iostream>
#include <map>

#include "GameObjects/ILevelBuilder.h"
#include "GameObjects/Level.h"
#include "GameObjects/LevelBuilderFromFile.h"

#include "Entities/IEntity.h"

#include "Utils/ILogger.h"
#include "Utils/Logger.h"
#include "Utils/Signals.h"

int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  auto logger = std::make_unique<PacMan::Utils::Logger>(
      "main", PacMan::Utils::LogLevel::INFO);
  logger->logInfo("Starting app");

  PacMan::GameObjects::ILevelBuilder *builder =
      new PacMan::GameObjects::LevelBuilderFromFile("Resources/Board1.txt");
  auto level = builder->release();

  for (const auto &row : level->getBoard()) {
    for (const auto &cell : row) {
      std::cout << *cell << " ";
    }
    std::cout << std::endl;
  }

  delete builder;

  return 0;
}