#pragma once

#include <array>
#include <string>
#include <vector>

namespace pacman::core::maps {

using TileRow = std::string;

struct Tile {
  using Unit = size_t;

  std::array<Unit, 2> pos = {0, 0};

  [[nodiscard]] Unit col() const { return pos[0]; }
  [[nodiscard]] Unit row() const { return pos[1]; }

  bool operator==(const Tile &) const = default;
};

struct GhostSpawns {
  Tile blinky = {};
  Tile pinky = {};
  Tile inky = {};
  Tile clyde = {};
};

struct Map {
  std::string name;
  Tile::Unit width = 0;
  Tile::Unit height = 0;
  float tileSize = 16.0f;
  std::vector<TileRow> tiles;
  std::vector<Tile> pacmanSpawns;
  GhostSpawns ghostSpawns;

  [[nodiscard]] std::string isValid() const {
    if (height == 0 || width == 0) {
      return "Map dimensions must be positive";
    }

    if (tiles.size() != height) {
      return "Tile row count (" + std::to_string(tiles.size()) +
             ") doesn't match height (" + std::to_string(height) + ")";
    }

    for (Tile::Unit r = 0; r < height; ++r) {
      if (tiles[r].size() != width) {
        return "Row " + std::to_string(r) + " width (" +
               std::to_string(tiles[r].size()) + ") doesn't match map width (" +
               std::to_string(width) + ")";
      }
    }

    auto inBounds = [&](Tile::Unit c, Tile::Unit r) {
      return c < width && r < height;
    };
    auto emptyField = [&](Tile::Unit c, Tile::Unit r) {
      return tileAt(c, r) == ' ';
    };

    for (const auto &spawn : pacmanSpawns) {
      const auto isInBounds = inBounds(spawn.col(), spawn.row());
      const auto isEmptyField = emptyField(spawn.col(), spawn.row());
      if (!isInBounds || !isEmptyField) {
        return "PacMan spawn (" + std::to_string(spawn.col()) + ", " +
               std::to_string(spawn.row()) + ") is illegal! (" +
               (!isInBounds ? "out of bounds" : "not an empty field") + ")";
      }
    }

    auto checkGhost = [&](const Tile &pos,
                          const std::string &name) -> std::string {
      if (pos.col() == 0 && pos.row() == 0) {
        return {};
      }
      if (!inBounds(pos.col(), pos.row())) {
        return name + " spawn (" + std::to_string(pos.col()) + ", " +
               std::to_string(pos.row()) + ") is out of bounds";
      } else if (!emptyField(pos.col(), pos.row())) {
        return name + " spawn (" + std::to_string(pos.col()) + ", " +
               std::to_string(pos.row()) + ") is not an empty field";
      }
      return {};
    };

    if (auto r = checkGhost(ghostSpawns.blinky, "Blinky"); !r.empty())
      return r;
    if (auto r = checkGhost(ghostSpawns.pinky, "Pinky"); !r.empty())
      return r;
    if (auto r = checkGhost(ghostSpawns.inky, "Inky"); !r.empty())
      return r;
    if (auto r = checkGhost(ghostSpawns.clyde, "Clyde"); !r.empty())
      return r;

    return {};
  }

  [[nodiscard]] char tileAt(Tile::Unit c, Tile::Unit r) const {
    if (r >= height || c >= width) {
      return '#';
    }
    return tiles[r][c];
  }
};

} // namespace pacman::core::maps