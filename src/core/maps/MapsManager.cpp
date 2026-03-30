#include "core/maps/MapsManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <cstdint>
#include <fstream>
#include <glaze/glaze.hpp>
#include <sstream>

namespace pacman::core::maps {

Utils::Logging::Logger &MapsManager::logger() {
    static Utils::Logging::Logger s_logger{"MapsManager"};
    return s_logger;
}

// Route LOG_X macros to the function-local static logger above.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define m_logger MapsManager::logger()

std::expected<Map, std::string> MapsManager::loadFromFile(const std::filesystem::path &path) {
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

std::expected<Map, std::string> MapsManager::loadFromJson(std::string_view json) {
    Map map;
    auto ec = glz::read_json(map, json);
    if (ec) {
        LOG_E("Failed to parse map JSON: error code {}", static_cast<uint32_t>(ec.ec));
        return std::unexpected("Failed to parse map JSON: error code " + std::to_string(static_cast<uint32_t>(ec.ec)));
    }

    auto validation = map.isValid();
    if (!validation.empty()) {
        LOG_E("Invalid map: {}", validation);
        return std::unexpected(validation);
    }

    LOG_I("Loaded map '{}' ({}x{})", map.name, map.width, map.height);
    return map;
}

std::expected<std::string, std::string> MapsManager::toJson(const Map &map) {
    auto result = glz::write_json(map);
    if (!result) {
        LOG_E("Failed to serialize map '{}' to JSON", map.name);
        return std::unexpected("Failed to serialize map to JSON");
    }
    return std::move(*result);
}

}  // namespace pacman::core::maps
