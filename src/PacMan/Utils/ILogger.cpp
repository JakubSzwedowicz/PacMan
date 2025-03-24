//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include "Utils/ILogger.h"

#include <spdlog/spdlog.h>

namespace PacMan {
namespace Utils {

std::unordered_set<ILogger *> ILogger::s_loggers;
std::string ILogger::s_logFileName = "log.txt";

ILogger::ILogger(const std::string &scopeName, LogLevel logLevel)
    : m_scopeName(scopeName), m_logLevel(logLevel) {
  s_loggers.insert(this);
}

ILogger::~ILogger() { s_loggers.erase(this); }
void ILogger::shutdownAll() {
  for (auto &logger : s_loggers) {
    logger->shutdown();
  }
  spdlog::shutdown();
}

void ILogger::setLogFileName(const std::string &logFileName) {
  s_logFileName = logFileName;
}

} // namespace Utils
} // namespace PacMan