#pragma once

#include <Utils/Logging/Logger.h>

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

#include "core/maps/Map.hpp"

namespace pacman::core::maps {

class MapsManager {
   public:
    static std::expected<Map, std::string> loadFromFile(const std::filesystem::path &path);

    static std::expected<Map, std::string> loadFromJson(std::string_view json);

    // Serialize map back to JSON string (inverse of loadFromJson).
    static std::expected<std::string, std::string> toJson(const Map &map);

   private:
    static std::expected<void, std::string> validate(const Map &map);

    static Utils::Logging::Logger &logger();
};

}  // namespace pacman::core::maps
