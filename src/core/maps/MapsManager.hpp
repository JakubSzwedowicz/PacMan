#pragma once

#include <Utils/Logging/LoggerSubscribed.h>

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

   private:
    static std::expected<void, std::string> validate(const Map &map);

    static inline Utils::Logging::LoggerSubscribed m_logger{"MapsManager"};
};

}  // namespace pacman::core::maps
