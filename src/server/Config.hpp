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

    // loggerConfig is extracted by ConfigManagerWithLogger to publish
    // LoggerConfig changes to all LoggerSubscribed instances.
    ConfigParameter<Utils::Logging::LoggerConfig> loggerConfig;
    ConfigParameter<std::string> configPath;
    ConfigParameter<int> port;
    ConfigParameter<std::string> mapPath;
    ConfigParameter<int> maxPlayers;
    ConfigParameter<float> tickRate;
    ConfigParameter<bool> renderAscii;
    // render_interval — CLI: --render-interval (only valid with --render-ascii)
    ConfigParameter<int> renderInterval;

    ServerConfig()
        : loggerConfig(m_container.buildConfigParam<Utils::Logging::LoggerConfig>(
              "logger_config", "Logger configuration", Utils::Logging::LoggerConfig{})),
          configPath(m_container.buildConfigParam<std::string>("config", "Path to server JSON config file",
                                                               "config/server.json")),
          port(m_container.buildConfigParam<int>("port", "Server listen port", 7777)),
          mapPath(
              m_container.buildConfigParam<std::string>("map_path", "Path to map JSON file", "assets/maps/test.json")),
          maxPlayers(m_container.buildConfigParam<int>("max_players", "Maximum number of players", 4)),
          tickRate(m_container.buildConfigParam<float>("tick_rate", "Server tick rate in Hz", 60.0f)),
          renderAscii(m_container.buildConfigParam<bool>("render_ascii", "Enable ASCII rendering to stdout", false)),
          renderInterval(m_container.buildConfigParam<int>(
              "render_interval", "ASCII render interval in ms (requires render_ascii)", 500)) {}
};

}  // namespace pacman::server

template <>
struct glz::meta<pacman::server::ServerConfig> {
    using T = pacman::server::ServerConfig;
    static constexpr auto value =
        glz::object("loggerConfig", &T::loggerConfig, "configPath", &T::configPath, "port", &T::port, "mapPath",
                    &T::mapPath, "maxPlayers", &T::maxPlayers, "tickRate", &T::tickRate, "renderAscii", &T::renderAscii,
                    "renderInterval", &T::renderInterval);
};
