#pragma once

#include <Logging/LogLevel.h>
#include <Utils/Config/ConfigParameters/ConfigParameter.h>
#include <Utils/Config/ConfigParameters/ConfigParametersContainer.h>
#include <Utils/Logging/LoggerConfig.h>

#include <glaze/glaze.hpp>
#include <string>

namespace pacman::client {

using Utils::Config::ConfigParameters::ConfigParameter;
using Utils::Config::ConfigParameters::ConfigParametersContainer;

struct ClientConfig {
    ConfigParametersContainer m_container;

    ConfigParameter<Utils::Logging::LoggerConfig> loggerConfig;
    ConfigParameter<std::string> configPath;
    ConfigParameter<int> windowWidth;
    ConfigParameter<int> windowHeight;
    ConfigParameter<std::string> playerName;
    ConfigParameter<std::string> serverAddress;
    ConfigParameter<int> serverPort;
    ConfigParameter<std::string> mapPath;

    ClientConfig()
        : loggerConfig(m_container.buildConfigParam<Utils::Logging::LoggerConfig>(
              "loggerConfig", "Logger configuration",
              Utils::Logging::LoggerConfig{.filename = "clientLog.txt",
                                           .globalLogLevel = Utils::Logging::LogLevel::INFO,
                                           .loggersLogLevels = {}})),
          configPath(m_container.buildConfigParam<std::string>("configPath", "Path to client JSON config file",
                                                               "config/client.json")),
          windowWidth(m_container.buildConfigParam<int>("windowWidth", "Window width in pixels", 800)),
          windowHeight(m_container.buildConfigParam<int>("windowHeight", "Window height in pixels", 600)),
          playerName(m_container.buildConfigParam<std::string>("playerName", "Player display name", "Player")),
          serverAddress(m_container.buildConfigParam<std::string>("serverAddress", "Server IP address", "127.0.0.1")),
          serverPort(m_container.buildConfigParam<int>("serverPort", "Server port number", 7777)),
          mapPath(m_container.buildConfigParam<std::string>("mapPath", "Path to map JSON file",
                                                            "assets/maps/classic.json")) {}
};

}  // namespace pacman::client

template <>
struct glz::meta<pacman::client::ClientConfig> {
    using T = pacman::client::ClientConfig;
    static constexpr auto value =
        glz::object("loggerConfig", &T::loggerConfig, "configPath", &T::configPath, "windowWidth", &T::windowWidth,
                    "windowHeight", &T::windowHeight, "playerName", &T::playerName, "serverAddress", &T::serverAddress,
                    "serverPort", &T::serverPort, "mapPath", &T::mapPath);
};
