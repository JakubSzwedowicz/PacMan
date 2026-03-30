#pragma once

#include <Utils/Logging/Logger.h>

#include <entt/entt.hpp>
#include <string>
#include <vector>

#include "core/maps/Map.hpp"

namespace pacman::server::render {

// Renders the game state to stdout as ASCII art.
// Intended for server-side debugging via --render-ascii flag.
//
// Legend:
//   #  wall
//   .  pellet
//   o  power pellet
//   C  PacMan
//   B  Blinky  P  Pinky  I  Inky  K  Clyde
//   F  frightened ghost
//   E  eaten ghost (returning to spawn)
class AsciiRenderer {
   public:
    AsciiRenderer();

    void render(const entt::registry &registry, const core::maps::Map &map);

   private:
    [[nodiscard]] std::vector<std::string> buildGrid(const entt::registry &registry, const core::maps::Map &map) const;

    void printGrid(const std::vector<std::string> &grid, const core::maps::Map &map) const;

    Utils::Logging::Logger m_logger{"AsciiRenderer"};
};

}  // namespace pacman::server::render
