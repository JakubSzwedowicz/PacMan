//
// Created by jakubszwedowicz on 3/25/25.
//

#ifndef GHOSTSTRATEGIES_H
#define GHOSTSTRATEGIES_H

#include <vector>

#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "Entities/MovingEntity.h"

namespace PacMan {
namespace GameLogic {
namespace Strategies {

using namespace ::PacMan::GameObjects::Entities;

// A* Pathfinding Algorithm
class PathFinder {
public:
  // Find path between two positions using A* algorithm
  static std::vector<TilePosition>
  findPath(const Position &start, const Position &goal, const Level &maze);

private:
  // Manhattan distance heuristic
  static float manhattanDistance(const Position &a, const Position &b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
  }

  // Reconstruct the path from start to goal
  static std::vector<Position> reconstructPath(
      const Position &start, const Position &goal,
      const std::unordered_map<Position, Position, Position::Hash> &came_from) {
    std::vector<Position> path;
    Position current = goal;

    // If no path found
    if (came_from.find(goal) == came_from.end()) {
      return path;
    }

    // Backtrack from goal to start
    while (!(current.x == start.x && current.y == start.y)) {
      path.push_back(current);
      current = came_from.at(current);
    }
    path.push_back(start);

    // Reverse to get start-to-goal order
    std::reverse(path.begin(), path.end());
    return path;
  }
};

class GhostStrategy {
public:
  virtual ~GhostStrategy() = default;
  virtual Position getTargetTile(const Ghost &ghost,
                                 const PacMan &pacMan) const = 0;
};

} // namespace Strategies
} // namespace GameLogic
} // namespace PacMan

#endif // GHOSTSTRATEGIES_H
