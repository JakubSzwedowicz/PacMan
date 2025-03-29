//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "Utils/Logger.h"

namespace PacMan {
namespace Utils {

Logger::Logger(const std::string &scopeName, LogLevel logLevel)
    : ILogger(scopeName, logLevel) {
  initLogger();
}

Logger::~Logger() {
  // ::shutdown is called automatically and that clears registered loggers, no
  // need to call drop_all or drop
}

void Logger::shutdown() {
  m_logger->critical("Logger {} shut down, flushing logger", m_scopeName);
  m_logger->flush();
}

void Logger::initLogger() {
  static auto fileSink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(s_logFileName, true);
  static std::atomic_bool isInitialized = false;
  if (!isInitialized) {
    fileSink->set_pattern("[%Y-%m-%d %H:%M:%S] [T:%t] [%l] [%n]: %v");
    spdlog::flush_every(std::chrono::seconds(1));
    isInitialized = true;
  }

  m_logger = std::make_shared<spdlog::logger>(m_scopeName, fileSink);
  spdlog::register_logger(m_logger);
}

void Logger::logDebug(const std::string &message) { m_logger->debug(message); }
void Logger::logInfo(const std::string &message) { m_logger->info(message); }
void Logger::logWarning(const std::string &message) { m_logger->warn(message); }
void Logger::logError(const std::string &message) { m_logger->error(message); }
void Logger::logCritical(const std::string &message) {
  m_logger->critical(message);
}

} // namespace Utils
} // namespace PacMan