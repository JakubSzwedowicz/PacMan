#pragma once

#include <Utils/Config/ConfigParameters/ConfigParameter.h>
#include <Utils/Config/ConfigParameters/ConfigParametersContainer.h>
#include <Utils/Logging/LoggerConfig.h>

#include <glaze/glaze.hpp>
#include <string>

namespace pacman::server {

using Utils::Config::ConfigParameters::ConfigParameter;
using Utils::Config::ConfigParameters::ConfigParametersContainer;

struct ServerConfig {
    ConfigParametersContainer m_container;

    // Picked up by ConfigManager via HasLoggerConfig concept and forwarded
    // to all Logger subscribers automatically on each config change.
    ConfigParameter<Utils::Logging::LoggerConfig> loggerConfig;
    ConfigParameter<std::string> configPath;
    ConfigParameter<int> port;
    ConfigParameter<std::string> mapPath;
    ConfigParameter<int> maxPlayers;
    ConfigParameter<float> tickRate;
    ConfigParameter<bool> renderAscii;
    // render_interval — CLI: --render-interval (only valid with --render-ascii)
    ConfigParameter<int> renderInterval;
    // CLI-only: fd to write "PORT=<n>\n" to after binding (set by ProcessSpawner via --notifyFd).
    ConfigParameter<int> notifyFd;

    ServerConfig()
        : loggerConfig(m_container.buildConfigParam<Utils::Logging::LoggerConfig>(
              "loggerConfig", "Logger configuration",
              Utils::Logging::LoggerConfig{.filename = "serverLog.txt",
                                           .globalLogLevel = Utils::Logging::LogLevel::INFO,
                                           .loggersLogLevels = {}})),
          configPath(m_container.buildConfigParam<std::string>("configPath", "Path to server JSON config file",
                                                               "config/server.json")),
          port(m_container.buildConfigParam<int>("port", "Server listen port", 7777)),
          mapPath(m_container.buildConfigParam<std::string>("mapPath", "Path to map JSON file",
                                                            "assets/maps/classic.json")),
          maxPlayers(m_container.buildConfigParam<int>("maxPlayers", "Maximum number of players", 4)),
          tickRate(m_container.buildConfigParam<float>("tickRate", "Server tick rate in Hz", 60.0f)),
          renderAscii(m_container.buildConfigParam<bool>("renderAscii", "Enable ASCII rendering to stdout", false)),
          renderInterval(m_container.buildConfigParam<int>(
              "renderInterval", "ASCII render interval in ms (requires render_ascii)", 500)),
          notifyFd(m_container.buildConfigParam<int>(
              "notifyFd", "fd to write bound port to after startup (-1 = disabled)", -1)) {}
};

}  // namespace pacman::server

template <>
struct glz::meta<pacman::server::ServerConfig> {
    using T = pacman::server::ServerConfig;
    static constexpr auto value =
        glz::object("loggerConfig", &T::loggerConfig, "configPath", &T::configPath, "port", &T::port, "mapPath",
                    &T::mapPath, "maxPlayers", &T::maxPlayers, "tickRate", &T::tickRate, "renderAscii", &T::renderAscii,
                    "renderInterval", &T::renderInterval, "notifyFd", &T::notifyFd);
};
