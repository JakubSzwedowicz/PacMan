#include "server/render/AsciiRenderer.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <cstdio>
#include <string>

#include "core/ecs/Components.hpp"

namespace pacman::server::render {

AsciiRenderer::AsciiRenderer() {}

void AsciiRenderer::render(const entt::registry &registry, const core::maps::Map &map) {
    auto grid = buildGrid(registry, map);
    printGrid(grid, map);
}

std::vector<std::string> AsciiRenderer::buildGrid(const entt::registry &registry, const core::maps::Map &map) const {
    // Start with walls; everything else is initially blank.
    std::vector<std::string> grid(map.height, std::string(map.width, ' '));
    for (core::maps::Tile::Unit r = 0; r < map.height; ++r) {
        for (core::maps::Tile::Unit c = 0; c < map.width; ++c) {
            const auto tt = map.tileTypeAt(c, r);
            if (tt == core::maps::TileType::Wall)
                grid[r][c] = '#';
            else if (tt == core::maps::TileType::GhostDoor)
                grid[r][c] = '=';
        }
    }

    float ts = map.tileSize;
    auto toCol = [ts](float x) { return static_cast<core::maps::Tile::Unit>(x / ts); };
    auto toRow = [ts](float y) { return static_cast<core::maps::Tile::Unit>(y / ts); };
    auto inBounds = [&](core::maps::Tile::Unit c, core::maps::Tile::Unit r) { return c < map.width && r < map.height; };

    // Pellets
    for (auto e : registry.view<const core::ecs::Position, const core::ecs::PelletTag>()) {
        const auto &pos = registry.get<const core::ecs::Position>(e);
        core::maps::Tile::Unit c = toCol(pos.x), r = toRow(pos.y);
        if (inBounds(c, r)) grid[r][c] = '.';
    }

    // Power pellets
    for (auto e : registry.view<const core::ecs::Position, const core::ecs::PowerPelletTag>()) {
        const auto &pos = registry.get<const core::ecs::Position>(e);
        core::maps::Tile::Unit c = toCol(pos.x), r = toRow(pos.y);
        if (inBounds(c, r)) grid[r][c] = 'o';
    }

    // Ghosts
    for (auto e : registry.view<const core::ecs::Position, const core::ecs::GhostState, const core::ecs::GhostTag>()) {
        const auto &pos = registry.get<const core::ecs::Position>(e);
        const auto &gs = registry.get<const core::ecs::GhostState>(e);
        core::maps::Tile::Unit c = toCol(pos.x), r = toRow(pos.y);
        if (!inBounds(c, r)) continue;

        char ch = '?';
        switch (gs.mode) {
            case core::ecs::GhostState::Mode::Frightened:
                ch = 'F';
                break;
            case core::ecs::GhostState::Mode::Eaten:
                ch = 'E';
                break;
            case core::ecs::GhostState::Mode::InHouse:
            case core::ecs::GhostState::Mode::Exiting:
            case core::ecs::GhostState::Mode::Chase:
            case core::ecs::GhostState::Mode::Scatter:
                switch (gs.type) {
                    case core::ecs::GhostType::Blinky:
                        ch = 'B';
                        break;
                    case core::ecs::GhostType::Pinky:
                        ch = 'P';
                        break;
                    case core::ecs::GhostType::Inky:
                        ch = 'I';
                        break;
                    case core::ecs::GhostType::Clyde:
                        ch = 'K';
                        break;
                }
                break;
        }
        grid[r][c] = ch;
    }

    // PacMan (drawn last — always visible on top)
    for (auto e : registry.view<const core::ecs::Position, const core::ecs::PacManTag>()) {
        const auto &pos = registry.get<const core::ecs::Position>(e);
        core::maps::Tile::Unit c = toCol(pos.x), r = toRow(pos.y);
        if (inBounds(c, r)) grid[r][c] = 'C';
    }

    return grid;
}

void AsciiRenderer::printGrid(const std::vector<std::string> &grid, const core::maps::Map &map) const {
    // ANSI: clear screen + move cursor to top-left.
    std::fputs("\033[2J\033[H", stdout);
    std::fprintf(stdout, "Map: %s  (%zux%zu)\n", map.name.c_str(), map.width, map.height);
    std::fputs("Legend: # wall  = ghost door  . pellet  o power  C pacman  B/P/I/K ghost  F frightened  E eaten\n\n",
               stdout);
    for (const auto &row : grid) {
        std::fputs(row.c_str(), stdout);
        std::fputc('\n', stdout);
    }
    std::fflush(stdout);
}

}  // namespace pacman::server::render
