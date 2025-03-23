//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef ILOGGER_H
#define ILOGGER_H

#include <string>
#include <unordered_set>

namespace PacMan {
namespace Utils {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class ILogger {
public:
  ILogger(const std::string &scopeName, LogLevel logLevel);
  virtual ~ILogger();
  virtual void shutdown() = 0;

  virtual void logDebug(const std::string &message) = 0;
  virtual void logInfo(const std::string &message) = 0;
  virtual void logWarning(const std::string &message) = 0;
  virtual void logError(const std::string &message) = 0;
  virtual void logCritical(const std::string &message) = 0;

  bool operator==(const ILogger &other) const {
    return m_scopeName == other.m_scopeName && m_logLevel == other.m_logLevel;
  }

  static void shutdownAll();

protected:
  const std::string m_scopeName;
  const LogLevel m_logLevel;

private:
  static std::unordered_set<ILogger *> s_loggers;
};

} // namespace Utils
} // namespace PacMan

#endif // ILOGGER_H
