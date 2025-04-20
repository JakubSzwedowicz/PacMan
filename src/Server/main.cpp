//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <csignal>

#include "Server/Server.h"
#include "Utils/ILogger.h"
#include "Utils/Logger.h"
#include "Utils/Signals.h"

#include <GameEventsManager/BasicGameEventsPublishers.h>

int main() {
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);
  std::signal(SIGTERM, PacMan::Utils::shutdownLoggers);
  std::signal(SIGKILL, PacMan::Utils::shutdownLoggers);

  PacMan::Utils::ILogger::setLogFileName("ServerLogs.txt");
  auto logger = std::make_unique<PacMan::Utils::Logger>(
      "main", PacMan::Utils::LogLevel::DEBUG);

  logger->logInfo("Launching Server");
  auto baseGameEventsManager =
      PacMan::GameEvents::getDefaultGameEventsManager();
  PacMan::Server::Server server(baseGameEventsManager);
  server.debugMain();
  return 0;
}