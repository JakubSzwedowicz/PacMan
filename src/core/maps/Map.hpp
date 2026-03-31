#pragma once

#include <array>
#include <glaze/glaze.hpp>
#include <string>
#include <vector>

namespace pacman::core::maps {

using TileRow = std::string;

// Strongly-typed representation of a single map tile.
// The JSON format still stores tiles as a string of chars; this enum is the
// runtime representation used by all gameplay/rendering code.
enum class TileType : uint8_t {
    Wall,         // '#'
    Empty,        // ' ' or any unrecognised character
    Pellet,       // '.'
    PowerPellet,  // 'o'
    GhostDoor,    // 'G' — passable only by ghosts; marks the ghost house exit
};

[[nodiscard]] inline TileType charToTileType(char c) noexcept {
    switch (c) {
        case '#':
            return TileType::Wall;
        case '.':
            return TileType::Pellet;
        case 'o':
            return TileType::PowerPellet;
        case 'G':
            return TileType::GhostDoor;
        default:
            return TileType::Empty;
    }
}

[[nodiscard]] inline char tileTypeToChar(TileType t) noexcept {
    switch (t) {
        case TileType::Wall:
            return '#';
        case TileType::Pellet:
            return '.';
        case TileType::PowerPellet:
            return 'o';
        case TileType::GhostDoor:
            return 'G';
        case TileType::Empty:
            return ' ';
    }
    return ' ';  // unreachable, satisfies compiler
}

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
    int maxPlayers = 0;  // 0 = unlimited (capped by pacmanSpawns.size())
    std::vector<TileRow> tiles;
    std::vector<Tile> pacmanSpawns;
    GhostSpawns ghostSpawns;
    // Derived in loadFromJson by scanning for the first 'G' tile; {0,0} means no ghost house.
    Tile ghostHouseExit = {};

    [[nodiscard]] std::string isValid() const {
        if (height == 0 || width == 0) {
            return "Map dimensions must be positive";
        }

        if (tiles.size() != height) {
            return "Tile row count (" + std::to_string(tiles.size()) + ") doesn't match height (" +
                   std::to_string(height) + ")";
        }

        for (Tile::Unit r = 0; r < height; ++r) {
            if (tiles[r].size() != width) {
                return "Row " + std::to_string(r) + " width (" + std::to_string(tiles[r].size()) +
                       ") doesn't match map width (" + std::to_string(width) + ")";
            }
        }

        auto inBounds = [&](Tile::Unit c, Tile::Unit r) { return c < width && r < height; };
        auto emptyField = [&](Tile::Unit c, Tile::Unit r) { return tileTypeAt(c, r) != TileType::Wall; };

        for (const auto &spawn : pacmanSpawns) {
            const auto isInBounds = inBounds(spawn.col(), spawn.row());
            const auto isEmptyField = emptyField(spawn.col(), spawn.row());
            if (!isInBounds || !isEmptyField) {
                return "PacMan spawn (" + std::to_string(spawn.col()) + ", " + std::to_string(spawn.row()) +
                       ") is illegal! (" + (!isInBounds ? "out of bounds" : "not an empty field") + ")";
            }
        }

        auto checkGhost = [&](const Tile &pos, const std::string &name) -> std::string {
            if (pos.col() == 0 && pos.row() == 0) {
                return {};
            }
            if (!inBounds(pos.col(), pos.row())) {
                return name + " spawn (" + std::to_string(pos.col()) + ", " + std::to_string(pos.row()) +
                       ") is out of bounds";
            } else if (!emptyField(pos.col(), pos.row())) {
                return name + " spawn (" + std::to_string(pos.col()) + ", " + std::to_string(pos.row()) +
                       ") is not an empty field";
            }
            return {};
        };

        if (auto r = checkGhost(ghostSpawns.blinky, "Blinky"); !r.empty()) return r;
        if (auto r = checkGhost(ghostSpawns.pinky, "Pinky"); !r.empty()) return r;
        if (auto r = checkGhost(ghostSpawns.inky, "Inky"); !r.empty()) return r;
        if (auto r = checkGhost(ghostSpawns.clyde, "Clyde"); !r.empty()) return r;

        // Validate ghost house exit: if it exists, the tile directly above must be passable.
        // Ghosts exit by moving up from the door, so this must be a valid escape route.
        if (ghostHouseExit.col() != 0 || ghostHouseExit.row() != 0) {
            if (ghostHouseExit.row() == 0) {
                return "Ghost house exit (" + std::to_string(ghostHouseExit.col()) + ", " +
                       std::to_string(ghostHouseExit.row()) + ") cannot be at the top of the map";
            }
            auto exitAbove = Tile{{ghostHouseExit.col(), ghostHouseExit.row() - 1}};
            if (!emptyField(exitAbove.col(), exitAbove.row())) {
                return "Ghost house exit requires a passable tile directly above (" +
                       std::to_string(exitAbove.col()) + ", " + std::to_string(exitAbove.row()) + ")";
            }
        }

        return {};
    }

    // Preferred API — returns a strongly-typed TileType.
    // Precondition: c < width && r < height — caller is responsible for bounds checking.
    [[nodiscard]] TileType tileTypeAt(Tile::Unit c, Tile::Unit r) const noexcept { return charToTileType(tiles[r][c]); }

    // Raw char access — kept for AsciiRenderer display output only.
    // Precondition: c < width && r < height — caller is responsible for bounds checking.
    [[nodiscard]] char tileAt(Tile::Unit c, Tile::Unit r) const noexcept { return tiles[r][c]; }
};

}  // namespace pacman::core::maps

// width and height are intentionally excluded — they are derived from tiles
// in MapsManager::loadFromJson and must never be stored in JSON.
template <>
struct glz::meta<pacman::core::maps::Map> {
    using T = pacman::core::maps::Map;
    static constexpr auto value =
        glz::object("name", &T::name, "tileSize", &T::tileSize, "maxPlayers", &T::maxPlayers, "tiles", &T::tiles,
                    "pacmanSpawns", &T::pacmanSpawns, "ghostSpawns", &T::ghostSpawns);
};