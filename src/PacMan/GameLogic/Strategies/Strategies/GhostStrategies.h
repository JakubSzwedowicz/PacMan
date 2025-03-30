//
// Created by jakubszwedowicz on 3/25/25.
//

#ifndef GHOSTSTRATEGIES_H
#define GHOSTSTRATEGIES_H

#include "Entities/Entity.h"
#include "Entities/MovingEntity.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameObjects {
class Level;
namespace Entities {
class Ghost;
class PacMan;
} // namespace Entities
} // namespace GameObjects

namespace GameLogic {
namespace Strategies {

using namespace GameObjects;
using namespace GameObjects::Entities;

struct StrategyContext {
  const Ghost &askingGhost;
  const PacMan &pacMan;
  const Level &level;
  TilePosition blinkyPosition; // Blinky's current position (needed by Inky)
};

TilePosition getValidTileInDirectionAndDistance(
    const StrategyContext &context, TilePosition target,
    GameObjects::Entities::EntityDirection direction, int distance);

TilePosition getValidTileClosestToTarget(const StrategyContext &context,
                                         const TilePosition target);
/**
 * @brief Abstract base class for Ghost targeting strategies.
 */
class IGhostStrategy {
public:
  virtual ~IGhostStrategy() = default;

  /**
   * @brief Calculates the target tile for the ghost based on the strategy.
   * @param context Game state information needed for the calculation.
   * @return The TilePosition the ghost should target.
   */
  [[nodiscard]] virtual TilePosition
  getTargetTile(const StrategyContext &context) const = 0;

  [[nodiscard]] virtual std::unique_ptr<IGhostStrategy> clone() const = 0;
};

// --- Concrete Strategy Declarations ---
class BlinkyChaseStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition
  getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;
};

class PinkyChaseStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition
  getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;
};

class InkyChaseStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition
  getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;
};

class ClydeChaseStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition
  getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;

private:
  // Define Clyde's proximity threshold and scatter target here or pass via
  // constructor
  static constexpr float CLYDE_THRESHOLD_DISTANCE = 8.0f;
};

class ScatterStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition
  getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;
};

class EatenStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;
};

class FrightenedStrategy : public IGhostStrategy {
public:
  [[nodiscard]] TilePosition
  getTargetTile(const StrategyContext &context) const override;
  [[nodiscard]] std::unique_ptr<IGhostStrategy> clone() const override;
};

// Frightened Strategy: Note - Frightened movement is often random-like at
// junctions, not targeting a specific tile. This might be handled directly in
// Ghost::update when state is FRIGHTENED, rather than using the strategy
// pattern here. If you wanted a strategy, it might return an invalid position
// or a special marker.

} // namespace Strategies
} // namespace GameLogic
} // namespace PacMan

#endif // GHOSTSTRATEGIES_H
