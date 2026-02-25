#include "core/maps/MapsManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <cstdint>
#include <fstream>
#include <sstream>

#include <glaze/glaze.hpp>

namespace pacman::core::maps {

std::expected<Map, std::string>
MapsManager::loadFromFile(const std::filesystem::path &path) {
  LOG_I("Loading map from {}", path.string());

  std::ifstream file(path);
  if (!file.is_open()) {
    LOG_E("Failed to open map file: {}", path.string());
    return std::unexpected("Failed to open map file: " + path.string());
  }

  std::ostringstream ss;
  ss << file.rdbuf();
  return loadFromJson(ss.str());
}

std::expected<Map, std::string>
MapsManager::loadFromJson(std::string_view json) {
  Map map;
  auto ec = glz::read_json(map, json);
  if (ec) {
    LOG_E("Failed to parse map JSON: error code {}",
          static_cast<uint32_t>(ec.ec));
    return std::unexpected("Failed to parse map JSON: error code " +
                           std::to_string(static_cast<uint32_t>(ec.ec)));
  }

  auto validation = map.isValid();
  if (!validation.empty()) {
    LOG_E("Invalid map: {}", validation);
    return std::unexpected(validation);
  }

  LOG_I("Loaded map '{}' ({}x{})", map.name, map.width, map.height);
  return map;
}

} // namespace pacman::core::maps
