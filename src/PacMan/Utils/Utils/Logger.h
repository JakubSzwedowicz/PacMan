//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <string>

#include "ILogger.h"

namespace spdlog {
class logger;
}

namespace PacMan {
namespace Utils {

class Logger final : public ILogger {
public:
  Logger(const std::string &scopeName, LogLevel logLevel);
  ~Logger();

  void shutdown() override;
  void logDebug(const std::string &message) override;
  void logInfo(const std::string &message) override;
  void logWarning(const std::string &message) override;
  void logError(const std::string &message) override;
  void logCritical(const std::string &message) override;

private:
  void initLogger();
  std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace Utils
} // namespace PacMan

#endif // LOGGER_H
