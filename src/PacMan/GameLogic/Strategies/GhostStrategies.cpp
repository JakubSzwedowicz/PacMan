//
// Created by jakubszwedowicz on 3/25/25.
//

#include "Strategies/GhostStrategies.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "PathFinders/AStarPathFinder.h"

namespace PacMan {
namespace GameLogic {
namespace Strategies {

TilePosition getValidTileInDirectionAndDistance(
    const StrategyContext &context, TilePosition target,
    GameObjects::Entities::EntityDirection direction, int distance) {
  while (distance--) {
    TilePosition nextMove = target;
    switch (direction) {
    case EntityDirection::UP:
      nextMove.y -= 1;
      break;
    case EntityDirection::DOWN:
      nextMove.y += 1;
      break;
    case EntityDirection::LEFT:
      nextMove.x -= 1;
      break;
    case EntityDirection::RIGHT:
      nextMove.x += 1;
      break;
    case EntityDirection::NONE:
      nextMove.y -= 1;
      break;
    }
    if (context.level.isTilePositionValid(nextMove)) {
      target = nextMove;
    } else {
      target = context.level.getValidAdjacentPositions(target).front();
    }
  }
  return target;
}

TilePosition getValidTileClosestToTarget(const StrategyContext &context,
                                         const TilePosition target) {
  if (!context.level.isTileInsideTheBoard(target)) {
    // fallback strategy if target is outside the board
    return getValidTileInDirectionAndDistance(
        context, context.pacMan.getTilePosition(),
        context.pacMan.getCurrDirection(), 4);
  }

  std::vector<TilePosition> toCheck;
  std::unordered_set<TilePosition, TilePosition::Hash> isVisited;

  toCheck.push_back(target);
  isVisited.insert(target);

  while (true) {
    std::vector<TilePosition> nextToCheck;
    while (!toCheck.empty()) {
      if (context.level.getEntityOnTile(toCheck.back()) != EntityType::WALL) {
        return toCheck.back();
      }
      const std::vector<TilePosition> offsets = {
          {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
      for (const auto &offset : offsets) {
        auto nextPos = toCheck.back() + offset;
        if (context.level.isTileInsideTheBoard(nextPos) &&
            isVisited.count(nextPos) == 0) {
          isVisited.insert(nextPos);
          nextToCheck.push_back(nextPos);
        }
      }
      toCheck.pop_back();
    }
    toCheck = std::move(nextToCheck);
  }
}

Entities::TilePosition
BlinkyChaseStrategy::getTargetTile(const StrategyContext &context) const {
  // Blinky directly targets Pac-Man's current tile.
  return context.pacMan.getTilePosition();
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> BlinkyChaseStrategy::clone() const {
  return std::make_unique<BlinkyChaseStrategy>(*this);
}

Entities::TilePosition
PinkyChaseStrategy::getTargetTile(const StrategyContext &context) const {
  // Pinky targets 4 tiles ahead of Pac-Man's current direction.
  EntityDirection pacManDir = context.pacMan.getCurrDirection();
  TilePosition target = context.pacMan.getTilePosition();
  constexpr int pinkyLeadTiles = 4;

  return getValidTileInDirectionAndDistance(context, target, pacManDir,
                                            pinkyLeadTiles);
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> PinkyChaseStrategy::clone() const {
  return std::make_unique<PinkyChaseStrategy>(*this);
}

TilePosition
InkyChaseStrategy::getTargetTile(const StrategyContext &context) const {
  // Inky targets based on Pac-Man and Blinky's positions.
  TilePosition pacManPos = context.pacMan.getTilePosition();
  EntityDirection pacManDir = context.pacMan.getCurrDirection();
  TilePosition blinkyPos = context.blinkyPosition;

  constexpr int INKY_LEAD_TILES = 2;
  TilePosition twoTilesInFrontOfPacMan = getValidTileInDirectionAndDistance(
      context, pacManPos, pacManDir, INKY_LEAD_TILES);

  // Calculate vector from Blinky to the intermediate tile
  TilePosition vecFromZeroToPoint = twoTilesInFrontOfPacMan - blinkyPos;
  // Double the vector from Blinky's position to get the final target
  TilePosition target = {blinkyPos.x + (2 * vecFromZeroToPoint.x),
                         blinkyPos.y + (2 * vecFromZeroToPoint.y)};

  return getValidTileClosestToTarget(context, target);
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> InkyChaseStrategy::clone() const {
  return std::make_unique<InkyChaseStrategy>(*this);
}

TilePosition
ClydeChaseStrategy::getTargetTile(const StrategyContext &context) const {
  // Clyde chases Pac-Man when far, but targets his scatter corner when close.
  TilePosition pacManPos = context.pacMan.getTilePosition();
  TilePosition ghostPos = context.askingGhost.getTilePosition();

  float distance = PathFinders::Heuristics::manhattanDistance(ghostPos, pacManPos);

  if (distance > CLYDE_THRESHOLD_DISTANCE) {
    // Far away: Chase Pac-Man directly (like Blinky)
    return pacManPos;
  } else {
    // Close: Target scatter corner
    return context.level.getGhostScatterPositions().clydeScatterPositionBottomLeft;
  }
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> ClydeChaseStrategy::clone() const {
  return std::make_unique<ClydeChaseStrategy>(*this);
}

TilePosition
ScatterStrategy::getTargetTile(const StrategyContext &context) const {
  // Target the fixed scatter corner based on the ghost's type.
  auto type =
      context.askingGhost
          .getGhostType();

  const auto& scatterPositions = context.level.getGhostScatterPositions();
  switch (type) {
  case GhostType::BLINKY:
    return scatterPositions.blinkyScatterPositionTopRight;
  case GhostType::PINKY:
    return scatterPositions.pinkyScatterPositionTopLeft;
  case GhostType::INKY:
    return scatterPositions.inkyScatterPositionBottomRight;
  case GhostType::CLYDE:
    return scatterPositions.clydeScatterPositionBottomLeft;
  default:
    return scatterPositions.clydeScatterPositionBottomLeft;
  }
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> ScatterStrategy::clone() const {
  return std::make_unique<ScatterStrategy>(*this);
}

TilePosition
EatenStrategy::getTargetTile(const StrategyContext &context) const {
  // Eaten ghosts always target the Ghost House entrance.
  return context.askingGhost.getStartingPosition();
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> EatenStrategy::clone() const {
  return std::make_unique<EatenStrategy>(*this);
}

TilePosition
FrightenedStrategy::getTargetTile(const StrategyContext &context) const {
  auto options = context.level.getValidAdjacentPositions(context.askingGhost.getTilePosition());
  if (options.empty()) {
    // This possible is rather impossible to occur unless on invalid map.
    return context.askingGhost.getScatteringPosition();
  }
  int id = std::rand() % options.size();
  return options[id];
}

[[nodiscard]] std::unique_ptr<IGhostStrategy> FrightenedStrategy::clone() const {
  return std::make_unique<FrightenedStrategy>(*this);
}

} // namespace Strategies
} // namespace GameLogic
} // namespace PacMan