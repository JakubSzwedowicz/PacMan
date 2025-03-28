//
// Created by jakubszwedowicz on 3/25/25.
//

namespace PacMan {
namespace GameLogic {
namespace Strategies {

#include "Strategies/GhostStrategies.h"

std::vector<GameObjects::Entities::TilePosition> PacMan::GameLogic::Strategies::PathFinder::findPath(
    const Position &start, const Position &goal, const Level &maze) {
  {
    // Priority queue for A* frontier
    auto comparator = [](const std::pair<Position, float> &a,
                         const std::pair<Position, float> &b) {
      return a.second > b.second;
    };
    std::priority_queue<std::pair<Position, float>,
                        std::vector<std::pair<Position, float>>,
                        decltype(comparator)>
        frontier(comparator);

    // Tracking visited and paths
    std::unordered_map<Position, Position, Position::Hash> came_from;
    std::unordered_map<Position, float, Position::Hash> cost_so_far;

    // Initialize start
    frontier.push({start, 0});
    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
      Position current = frontier.top().first;
      frontier.pop();

      // Reached goal
      if (current.x == goal.x && current.y == goal.y) {
        break;
      }

      // Check adjacent positions
      for (const Position &next : maze.getAdjacentPositions(current)) {
        // Calculate new cost
        float new_cost = cost_so_far[current] + 1;

        // Determine if we should explore this path
        if (cost_so_far.find(next) == cost_so_far.end() ||
            new_cost < cost_so_far[next]) {

          // Heuristic: Manhattan distance
          float priority = new_cost + manhattanDistance(next, goal);

          frontier.push({next, priority});
          cost_so_far[next] = new_cost;
          came_from[next] = current;
        }
      }
    }

    // Reconstruct path
    return reconstructPath(start, goal, came_from);
  }
}

} // namespace Strategies
} // namespace GameLogic
} // namespace PacMan