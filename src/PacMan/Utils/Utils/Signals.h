//
// Created by jakubszwedowicz on 3/23/25.
//

#ifndef SIGNALS_H
#define SIGNALS_H

#include <memory>

#include "Logger.h"

namespace PacMan::Utils {
inline void shutdownLoggers(int signal) {
  auto logger = std::make_unique<Logger>(
      "signalHandler", LogLevel::DEBUG);
  logger->logCritical("Signal " + std::to_string(signal) +
                      " received, shutting down loggers");

  ILogger::shutdownAll();

  // Continue shutting down a program
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}
} // namespace PacMan::Utils

#endif // SIGNALS_H
