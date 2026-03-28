#pragma once

#include <Utils/Config/ConfigParameters/ConfigParameter.h>
#include <Utils/Config/ConfigParameters/ConfigParametersContainer.h>
#include <Utils/Logging/LoggerConfig.h>

#include <glaze/glaze.hpp>

#include <string>

namespace pacman::core {

using Utils::Config::ConfigParameters::ConfigParameter;
using Utils::Config::ConfigParameters::ConfigParametersContainer;

// ---------------------------------------------------------------------------
// ClientConfig
// ---------------------------------------------------------------------------
struct ClientConfig {
  ConfigParametersContainer m_container;

  ConfigParameter<Utils::Logging::LoggerConfig> loggerConfig;
  ConfigParameter<int> windowWidth;
  ConfigParameter<int> windowHeight;
  ConfigParameter<std::string> playerName;
  ConfigParameter<std::string> serverAddress;
  ConfigParameter<int> serverPort;
  ConfigParameter<std::string> mapPath;

  ClientConfig()
      : loggerConfig(m_container.buildConfigParam<Utils::Logging::LoggerConfig>(
            "logger_config", "Logger configuration",
            Utils::Logging::LoggerConfig{})),
        windowWidth(m_container.buildConfigParam<int>(
            "window_width", "Window width in pixels", 800)),
        windowHeight(m_container.buildConfigParam<int>(
            "window_height", "Window height in pixels", 600)),
        playerName(m_container.buildConfigParam<std::string>(
            "player_name", "Player display name", "Player")),
        serverAddress(m_container.buildConfigParam<std::string>(
            "server_address", "Server IP address", "127.0.0.1")),
        serverPort(m_container.buildConfigParam<int>(
            "server_port", "Server port number", 7777)),
        mapPath(m_container.buildConfigParam<std::string>(
            "map_path", "Path to map JSON file", "assets/maps/test.json")) {}
};

// ---------------------------------------------------------------------------
// ServerConfig
// ---------------------------------------------------------------------------
struct ServerConfig {
  ConfigParametersContainer m_container;

  // loggerConfig is extracted by ConfigManagerWithLogger to publish
  // LoggerConfig changes to all LoggerSubscribed instances.
  ConfigParameter<Utils::Logging::LoggerConfig> loggerConfig;
  ConfigParameter<int> port;
  ConfigParameter<std::string> mapPath;
  ConfigParameter<int> maxPlayers;
  ConfigParameter<float> tickRate;
  ConfigParameter<bool> renderAscii;
  // render_interval — CLI: --render-interval (only valid with --render-ascii)
  ConfigParameter<int> renderInterval;

  ServerConfig()
      : loggerConfig(m_container.buildConfigParam<Utils::Logging::LoggerConfig>(
            "logger_config", "Logger configuration",
            Utils::Logging::LoggerConfig{})),
        port(m_container.buildConfigParam<int>(
            "port", "Server listen port", 7777)),
        mapPath(m_container.buildConfigParam<std::string>(
            "map_path", "Path to map JSON file", "assets/maps/test.json")),
        maxPlayers(m_container.buildConfigParam<int>(
            "max_players", "Maximum number of players", 4)),
        tickRate(m_container.buildConfigParam<float>(
            "tick_rate", "Server tick rate in Hz", 60.0f)),
        renderAscii(m_container.buildConfigParam<bool>(
            "render_ascii", "Enable ASCII rendering to stdout", false)),
        renderInterval(m_container.buildConfigParam<int>(
            "render_interval", "ASCII render interval in ms (requires render_ascii)", 500)) {}
};

} // namespace pacman::core

// ---------------------------------------------------------------------------
// Glaze meta — lists ConfigParameter fields only; m_container is internal.
// Config structs using ConfigParameter<T> cannot use default Glaze reflection
// because they require a user-provided constructor (non-aggregate in C++20).
// ---------------------------------------------------------------------------
template <>
struct glz::meta<pacman::core::ClientConfig> {
  using T = pacman::core::ClientConfig;
  static constexpr auto value =
      glz::object("loggerConfig",   &T::loggerConfig,
                  "windowWidth",   &T::windowWidth,
                  "windowHeight",  &T::windowHeight,
                  "playerName",    &T::playerName,
                  "serverAddress", &T::serverAddress,
                  "serverPort",    &T::serverPort,
                  "mapPath",       &T::mapPath);
};

template <>
struct glz::meta<pacman::core::ServerConfig> {
  using T = pacman::core::ServerConfig;
  static constexpr auto value =
      glz::object("loggerConfig",   &T::loggerConfig,
                  "port",           &T::port,
                  "mapPath",        &T::mapPath,
                  "maxPlayers",     &T::maxPlayers,
                  "tickRate",       &T::tickRate,
                  "renderAscii",    &T::renderAscii,
                  "renderInterval", &T::renderInterval);
};
