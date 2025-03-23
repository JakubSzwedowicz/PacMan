#include <csignal>
#include <execinfo.h> // For backtrace()
#include <iostream>
#include <map>

#include "GameObjects/ILevelBuilder.h"
#include "GameObjects/Level.h"
#include "GameObjects/LevelBuilderFromFile.h"

#include "Entities/IEntity.h"

#include "Utils/ILogger.h"
#include "Utils/Logger.h"

void shutdownLoggers(int signal) {
  auto logger = std::make_unique<PacMan::Utils::Logger>(
      "signalHandler", PacMan::Utils::LogLevel::DEBUG);
  logger->logCritical("Signal " + std::to_string(signal) +
                      " received, shutting down loggers");

  PacMan::Utils::ILogger::shutdownAll();

  // Continue shutting down a program
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

int main() {
  std::signal(SIGSEGV, shutdownLoggers);

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