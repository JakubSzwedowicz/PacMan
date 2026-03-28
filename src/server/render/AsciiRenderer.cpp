#include "server/render/AsciiRenderer.hpp"

#include "core/ecs/Components.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <cstdio>
#include <string>

namespace pacman::server::render {

AsciiRenderer::AsciiRenderer() {}

void AsciiRenderer::render(const entt::registry &registry,
                            const core::maps::Map &map) {
  auto grid = buildGrid(registry, map);
  printGrid(grid, map);
}

std::vector<std::string>
AsciiRenderer::buildGrid(const entt::registry &registry,
                          const core::maps::Map &map) const {
  // Start with walls; everything else is initially blank.
  std::vector<std::string> grid(map.height, std::string(map.width, ' '));
  for (size_t r = 0; r < map.height; ++r) {
    for (size_t c = 0; c < map.width; ++c) {
      if (map.tileAt(c, r) == '#') grid[r][c] = '#';
    }
  }

  float ts = map.tileSize;
  auto toCol = [ts](float x) { return static_cast<size_t>(x / ts); };
  auto toRow = [ts](float y) { return static_cast<size_t>(y / ts); };
  auto inBounds = [&](size_t c, size_t r) {
    return c < map.width && r < map.height;
  };

  // Pellets
  for (auto e :
       registry.view<const core::ecs::Position, const core::ecs::PelletTag>()) {
    const auto &pos =
        registry.get<const core::ecs::Position>(e);
    size_t c = toCol(pos.x), r = toRow(pos.y);
    if (inBounds(c, r)) grid[r][c] = '.';
  }

  // Power pellets
  for (auto e : registry.view<const core::ecs::Position,
                               const core::ecs::PowerPelletTag>()) {
    const auto &pos = registry.get<const core::ecs::Position>(e);
    size_t c = toCol(pos.x), r = toRow(pos.y);
    if (inBounds(c, r)) grid[r][c] = 'o';
  }

  // Ghosts
  for (auto e :
       registry.view<const core::ecs::Position, const core::ecs::GhostState,
                     const core::ecs::GhostTag>()) {
    const auto &pos = registry.get<const core::ecs::Position>(e);
    const auto &gs = registry.get<const core::ecs::GhostState>(e);
    size_t c = toCol(pos.x), r = toRow(pos.y);
    if (!inBounds(c, r)) continue;

    char ch = 'G';
    switch (gs.mode) {
    case core::ecs::GhostState::Mode::Frightened: ch = 'F'; break;
    case core::ecs::GhostState::Mode::Eaten:      ch = 'E'; break;
    default:
      switch (gs.type) {
      case core::ecs::GhostType::Blinky: ch = 'B'; break;
      case core::ecs::GhostType::Pinky:  ch = 'P'; break;
      case core::ecs::GhostType::Inky:   ch = 'I'; break;
      case core::ecs::GhostType::Clyde:  ch = 'K'; break;
      }
    }
    grid[r][c] = ch;
  }

  // PacMan (drawn last — always visible on top)
  for (auto e : registry.view<const core::ecs::Position,
                               const core::ecs::PacManTag>()) {
    const auto &pos = registry.get<const core::ecs::Position>(e);
    size_t c = toCol(pos.x), r = toRow(pos.y);
    if (inBounds(c, r)) grid[r][c] = 'C';
  }

  return grid;
}

void AsciiRenderer::printGrid(const std::vector<std::string> &grid,
                               const core::maps::Map &map) const {
  // ANSI: clear screen + move cursor to top-left.
  std::fputs("\033[2J\033[H", stdout);
  std::fprintf(stdout, "Map: %s  (%zux%zu)\n", map.name.c_str(), map.width,
               map.height);
  std::fputs("Legend: # wall  . pellet  o power  C pacman  B/P/I/K ghost  F "
             "frightened  E eaten\n\n",
             stdout);
  for (const auto &row : grid) {
    std::fputs(row.c_str(), stdout);
    std::fputc('\n', stdout);
  }
  std::fflush(stdout);
}

} // namespace pacman::server::render
