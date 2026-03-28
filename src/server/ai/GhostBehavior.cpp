#include "server/ai/GhostBehavior.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace pacman::server::ai {

namespace {

core::ecs::Direction opposite(core::ecs::Direction d) {
  switch (d) {
  case core::ecs::Direction::Up:    return core::ecs::Direction::Down;
  case core::ecs::Direction::Down:  return core::ecs::Direction::Up;
  case core::ecs::Direction::Left:  return core::ecs::Direction::Right;
  case core::ecs::Direction::Right: return core::ecs::Direction::Left;
  default:                          return core::ecs::Direction::None;
  }
}

core::maps::Tile tileFromPixel(float x, float y, float tileSize) {
  core::maps::Tile t;
  t.pos[0] = static_cast<core::maps::Tile::Unit>(x / tileSize);
  t.pos[1] = static_cast<core::maps::Tile::Unit>(y / tileSize);
  return t;
}

} // namespace

core::maps::Tile GhostBehavior::nearestPacManTile(const entt::registry &registry,
                                                   const core::maps::Map &map) {
  auto view =
      registry.view<const core::ecs::Position, const core::ecs::PacManTag>();
  for (auto e : view) {
    const auto &pos = view.get<const core::ecs::Position>(e);
    return tileFromPixel(pos.x, pos.y, map.tileSize);
  }
  return {}; // no PacMan in the registry (Phase 3 / server solo mode)
}

core::ecs::Direction
GhostBehavior::chooseDirection(const entt::registry & /*registry*/,
                                const core::maps::Map &map, float ghostX,
                                float ghostY, core::ecs::Direction currentDir,
                                const core::maps::Tile &targetTile) {
  float ts = map.tileSize;
  auto ghostTile = tileFromPixel(ghostX, ghostY, ts);

  struct Candidate {
    core::ecs::Direction dir;
    int dc, dr;
  };
  static constexpr std::array<Candidate, 4> candidates{{
      {core::ecs::Direction::Up,    0, -1},
      {core::ecs::Direction::Down,  0,  1},
      {core::ecs::Direction::Left, -1,  0},
      {core::ecs::Direction::Right, 1,  0},
  }};

  core::ecs::Direction best = core::ecs::Direction::None;
  float bestDist = std::numeric_limits<float>::max();

  for (const auto &cand : candidates) {
    if (cand.dir == opposite(currentDir)) continue;

    int nc = static_cast<int>(ghostTile.col()) + cand.dc;
    int nr = static_cast<int>(ghostTile.row()) + cand.dr;
    if (nc < 0 || nr < 0) continue;
    if (map.tileAt(static_cast<size_t>(nc), static_cast<size_t>(nr)) == '#')
      continue;

    float dx = static_cast<float>(nc) - static_cast<float>(targetTile.col());
    float dy = static_cast<float>(nr) - static_cast<float>(targetTile.row());
    float dist = dx * dx + dy * dy;

    if (dist < bestDist) {
      bestDist = dist;
      best = cand.dir;
    }
  }

  // If completely boxed in (e.g. just spawned), allow reversing.
  if (best == core::ecs::Direction::None) {
    for (const auto &cand : candidates) {
      int nc = static_cast<int>(ghostTile.col()) + cand.dc;
      int nr = static_cast<int>(ghostTile.row()) + cand.dr;
      if (nc < 0 || nr < 0) continue;
      if (map.tileAt(static_cast<size_t>(nc), static_cast<size_t>(nr)) != '#') {
        best = cand.dir;
        break;
      }
    }
  }

  return best;
}

core::maps::Tile GhostBehavior::blinkyTarget(const entt::registry &registry,
                                              const core::maps::Map &map) {
  return nearestPacManTile(registry, map);
}

core::maps::Tile GhostBehavior::pinkyTarget(const entt::registry &registry,
                                             const core::maps::Map &map) {
  float ts = map.tileSize;
  auto view = registry.view<const core::ecs::Position,
                             const core::ecs::DirectionState,
                             const core::ecs::PacManTag>();
  for (auto e : view) {
    const auto &pos = view.get<const core::ecs::Position>(e);
    const auto &dir = view.get<const core::ecs::DirectionState>(e);

    int c = static_cast<int>(pos.x / ts);
    int r = static_cast<int>(pos.y / ts);

    switch (dir.current) {
    case core::ecs::Direction::Up:    r -= 4; break;
    case core::ecs::Direction::Down:  r += 4; break;
    case core::ecs::Direction::Left:  c -= 4; break;
    case core::ecs::Direction::Right: c += 4; break;
    default: break;
    }

    core::maps::Tile t;
    t.pos[0] = static_cast<core::maps::Tile::Unit>(std::max(0, c));
    t.pos[1] = static_cast<core::maps::Tile::Unit>(std::max(0, r));
    return t;
  }
  return nearestPacManTile(registry, map);
}

core::maps::Tile GhostBehavior::inkyTarget(const entt::registry &registry,
                                            const core::maps::Map &map,
                                            float blinkyX, float blinkyY) {
  float ts = map.tileSize;
  auto view = registry.view<const core::ecs::Position,
                             const core::ecs::DirectionState,
                             const core::ecs::PacManTag>();
  for (auto e : view) {
    const auto &pos = view.get<const core::ecs::Position>(e);
    const auto &dir = view.get<const core::ecs::DirectionState>(e);

    int c = static_cast<int>(pos.x / ts);
    int r = static_cast<int>(pos.y / ts);

    // 2 tiles ahead of PacMan
    switch (dir.current) {
    case core::ecs::Direction::Up:    r -= 2; break;
    case core::ecs::Direction::Down:  r += 2; break;
    case core::ecs::Direction::Left:  c -= 2; break;
    case core::ecs::Direction::Right: c += 2; break;
    default: break;
    }

    // Reflect Blinky through that pivot point
    int blinkyC = static_cast<int>(blinkyX / ts);
    int blinkyR = static_cast<int>(blinkyY / ts);

    int tc = 2 * c - blinkyC;
    int tr = 2 * r - blinkyR;

    core::maps::Tile t;
    t.pos[0] = static_cast<core::maps::Tile::Unit>(std::max(0, tc));
    t.pos[1] = static_cast<core::maps::Tile::Unit>(std::max(0, tr));
    return t;
  }
  return nearestPacManTile(registry, map);
}

core::maps::Tile GhostBehavior::clydeTarget(const entt::registry &registry,
                                             const core::maps::Map &map,
                                             float clydeX, float clydeY) {
  float ts = map.tileSize;
  auto view = registry.view<const core::ecs::Position,
                             const core::ecs::PacManTag>();
  for (auto e : view) {
    const auto &pos = view.get<const core::ecs::Position>(e);

    float dc = pos.x / ts - clydeX / ts;
    float dr = pos.y / ts - clydeY / ts;
    float dist = std::sqrt(dc * dc + dr * dr);

    if (dist > 8.0f) {
      return tileFromPixel(pos.x, pos.y, ts);
    }
    // Scatter corner: bottom-left of map
    core::maps::Tile t;
    t.pos[0] = 0;
    t.pos[1] = map.height > 0 ? map.height - 1 : 0;
    return t;
  }
  return nearestPacManTile(registry, map);
}

} // namespace pacman::server::ai
