#include <csignal>

#include "Utils/ILogger.h"
#include "Utils/Signals.h"

int main() {
  PacMan::Utils::ILogger::setLogFileName("MainLogs.txt");
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);

  return 0;
}