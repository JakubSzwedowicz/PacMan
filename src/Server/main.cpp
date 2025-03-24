//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <csignal>

#include "Server/Server.h"
#include "Utils/ILogger.h"
#include "Utils/Logger.h"
#include "Utils/Signals.h"

int main() {
  std::signal(SIGSEGV, PacMan::Utils::shutdownLoggers);
  std::signal(SIGTERM, PacMan::Utils::shutdownLoggers);
  std::signal(SIGKILL, PacMan::Utils::shutdownLoggers);

  PacMan::Utils::ILogger::setLogFileName("ServerLogs.txt");
  auto logger = std::make_unique<PacMan::Utils::Logger>(
      "main", PacMan::Utils::LogLevel::DEBUG);

  PacMan::Server::Server server;
  server.main();
  return 0;
}