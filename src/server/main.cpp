#include "core/Config.hpp"
#include "server/ServerApp.hpp"

#include <Utils/Config/ConfigParser/JsonConfigParser.h>
#include <Utils/Logging/LoggerConfig.h>

#include <csignal>
#include <fstream>
#include <memory>

namespace {
pacman::server::ServerApp *globalServerApp = nullptr;

void signalHandler(int /*sig*/) {
  if (globalServerApp) {
    globalServerApp->stop();
  }
}
} // namespace

int main(int argc, char *argv[]) {
  auto loggerConfig = std::make_shared<Utils::Logging::LoggerConfig>();
  loggerConfig->globalLogLevel = Utils::Logging::LogLevel::DEBUG;

  pacman::core::ServerConfig config;

  std::string configPath = "config/server.json";
  if (argc > 2 && std::string(argv[1]) == "--config") {
    configPath = argv[2];
  }

  std::ifstream file(configPath);
  if (file.is_open()) {
    Utils::Config::JsonConfigParser<pacman::core::ServerConfig> parser;
    auto parsed = parser.readConfig(file);
    if (parsed) {
      config = *parsed;
    }
  }

  pacman::server::ServerApp app(std::move(config), std::move(loggerConfig));
  globalServerApp = &app;

  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  app.run();

  globalServerApp = nullptr;
  return 0;
}
