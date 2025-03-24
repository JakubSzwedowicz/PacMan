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

  return 0;
}