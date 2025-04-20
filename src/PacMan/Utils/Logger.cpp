//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <iostream>
#include <memory>
#include <ostream>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Utils/Logger.h"

namespace PacMan {
namespace Utils {

Logger::Logger(const std::string &scopeName, LogLevel logLevel)
    : ILogger(scopeName, logLevel) {
  initLogger();
}

Logger::~Logger() { shutdown(); }

void Logger::shutdown() {
  if (m_logger) {
    m_logger->info("Logger {} shut down, flushing logger", m_scopeName);
    m_logger->flush();
    m_logger = nullptr;
  }
}

void Logger::initLogger() {
  static std::vector<spdlog::sink_ptr> sinks;

  static std::atomic_bool isInitialized(false);
  if (!isInitialized.exchange(true)) {
    const auto logPattern = "[%Y-%m-%d %H:%M:%S] [T:%t] [%l] [%n]: %v";
    // File sink
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        s_logFileName, true);
    fileSink->set_pattern(logPattern);
    sinks.push_back(fileSink);

    // stdout sink
    // auto consoleSink =
    // std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    // consoleSink->set_pattern(logPattern);
    // sinks.push_back(consoleSink);

    spdlog::flush_every(std::chrono::seconds(1));
    spdlog::enable_backtrace(20);
  }

  m_logger =
      std::make_shared<spdlog::logger>(m_scopeName, sinks.begin(), sinks.end());
  switch (m_logLevel) {
  case LogLevel::DEBUG:
    m_logger->set_level(spdlog::level::debug);
    break;
  case LogLevel::INFO:
    m_logger->set_level(spdlog::level::info);
    break;
  case LogLevel::WARNING:
    m_logger->set_level(spdlog::level::warn);
    break;
  case LogLevel::ERROR:
    m_logger->set_level(spdlog::level::err);
    break;
  }
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