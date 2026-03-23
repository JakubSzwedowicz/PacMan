#pragma once

#include <string>

namespace pacman::core {

struct ClientConfig {
  int windowWidth = 800;
  int windowHeight = 600;
  std::string playerName = "Player";
  std::string serverAddress = "127.0.0.1";
  int serverPort = 7777;
  std::string mapPath = "assets/maps/test.json";
};

struct ServerConfig {
  int port = 7777;
  std::string mapPath = "assets/maps/test.json";
  int maxPlayers = 4;
  float tickRate = 60.0f;
};

} // namespace pacman::core
