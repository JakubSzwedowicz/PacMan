
#include <csignal>

#include "GameEventsManager/BasicGameEventsPublishers.h"
#include "Utils/ILogger.h"
#include "Utils/Signals.h"

using namespace PacMan::GameEvents;

int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  return 0;
}