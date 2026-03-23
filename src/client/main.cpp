#include "client/ClientApp.hpp"
#include "core/Config.hpp"

#include <Utils/Config/ConfigParser/JsonConfigParser.h>
#include <Utils/Logging/LoggerConfig.h>

#include <fstream>
#include <memory>

int main(int argc, char *argv[]) {
  auto loggerConfig = std::make_shared<Utils::Logging::LoggerConfig>();
  loggerConfig->globalLogLevel = Utils::Logging::LogLevel::DEBUG;

  pacman::core::ClientConfig config;

  std::string configPath = "config/client.json";
  if (argc > 2 && std::string(argv[1]) == "--config") {
    configPath = argv[2];
  }

  std::ifstream file(configPath);
  if (file.is_open()) {
    Utils::Config::JsonConfigParser<pacman::core::ClientConfig> parser;
    auto parsed = parser.readConfig(file);
    if (parsed) {
      config = *parsed;
    }
  }

  pacman::client::ClientApp app(std::move(config), std::move(loggerConfig));
  app.run();

  return 0;
}
