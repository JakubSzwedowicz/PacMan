//
// Created by jakubszwedowicz on 3/30/25.
//

#include <vector>

#include "Entities/Entity.h"
#include "GameObjects/Level.h"
#include "PathFinders/AStarPathFinder.h"

namespace PacMan {
namespace GameLogic {
namespace Strategies {
namespace PathFinders {

std::vector<TilePosition> AStarPathFinder::findPath(const TilePosition &start,
                                               const TilePosition &goal,
                                               const Level &maze) {
  // Priority queue for A* frontier (Nodes with lower f_cost have higher
  // priority)
  auto comparator = [](const std::pair<TilePosition, float> &a,
                       const std::pair<TilePosition, float> &b) {
    return a.second > b.second; // Min-heap based on f_cost (second element)
  };
  std::priority_queue<std::pair<TilePosition, float>,
                      std::vector<std::pair<TilePosition, float>>,
                      decltype(comparator)>
      frontier(comparator);

  // Tracking visited nodes, paths, and costs
  std::unordered_map<TilePosition, TilePosition, TilePosition::Hash> cameFrom;
  std::unordered_map<TilePosition, float, TilePosition::Hash> costSoFar;

  // Initialize start node
  frontier.emplace(start, 0.0f); // Priority = f_cost = g_cost + h_cost
  frontier.emplace(start, Heuristics::manhattanDistance(start, goal));
  cameFrom[start] = start;
  costSoFar[start] = 0.0f;

  while (!frontier.empty()) {
    TilePosition current = frontier.top().first;
    frontier.pop();

    if (current == goal) {
      break;
    }

    for (const TilePosition &next : maze.getValidAdjacentPositions(current)) {
      float newCost = costSoFar[current] + 1.0f;

      // If 'next' hasn't been visited or we found a cheaper path to it
      auto it = costSoFar.find(next);
      if (it == costSoFar.end() || newCost < it->second) {
        costSoFar[next] = newCost;
        float priority =
            newCost + Heuristics::manhattanDistance(next, goal); // f_cost = g_cost + h_cost
        frontier.emplace(next, priority);
        cameFrom[next] = current; // Record the path
      }
    }
  }

  // Reconstruct path from goal back to start
  return reconstructPath(start, goal, cameFrom);
}

std::vector<TilePosition> AStarPathFinder::reconstructPath(
    const TilePosition &start, const TilePosition &goal,
    const std::unordered_map<TilePosition, TilePosition, TilePosition::Hash>
        &cameFrom) {
  std::vector<TilePosition> path;
  TilePosition current = goal;

  if (cameFrom.find(goal) == cameFrom.end() && start != goal) {
    return path; // Return empty path if goal not found (and start != goal)
  }

  while (current != start) {
    path.push_back(current);
    auto it = cameFrom.find(current);
    if (it == cameFrom.end()) {
      // m_logger->logError("Error in A* algorithm!");
      return path;
    }
    current = it->first;
  }

  // The path is currently goal -> neighbour of start, reverse it
  std::reverse(path.begin(), path.end());

  return path;
}

} // namespace PathFinders
} // namespace Strategies
} // namespace GameLogic
} // namespace PacMan