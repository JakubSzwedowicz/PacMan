//
// Created by jakubszwedowicz on 3/30/25.
//

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include "Entities/Entity.h"

namespace PacMan {

namespace GameObjects {
class Level;
}

namespace GameLogic {
namespace Strategies {
namespace PathFinders {

using namespace GameObjects;
using namespace GameObjects::Entities;

class Heuristics {
public:
  static float manhattanDistance(const TilePosition &a, const TilePosition &b) {
    return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
  }
};

class AStarPathFinder {
public:
  static std::vector<TilePosition> findPath(const TilePosition &start,
                                            const TilePosition &goal,
                                            const Level &maze);

private:
  static std::vector<TilePosition> reconstructPath(
      const TilePosition &start, const TilePosition &goal,
      const std::unordered_map<TilePosition, TilePosition, TilePosition::Hash>
          &cameFrom);
};

} // namespace PathFinders
} // namespace Strategies
} // namespace GameLogic
} // namespace PacMan

#endif // PATHFINDER_H
