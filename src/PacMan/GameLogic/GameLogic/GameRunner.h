//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef GAMERUNNER_H
#define GAMERUNNER_H

#include "GameStatus.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "GameObjects/Level.h"
#include "Utils/Logger.h"

namespace PacMan {
namespace GameLogic {

using namespace GameObjects;
using namespace Entities;
class GameRunner {
public:
  GameRunner(int gameId, std::unique_ptr<GameObjects::Level> level);
  bool startGame();

  int getGameId() const { return m_gameId; }

private:
  void gameLoop();
  void update(std::chrono::milliseconds deltaTime);
  void updateMovingEntity(MovingEntity& movingEntity, std::chrono::milliseconds deltaTime);
  void updateGhost(Ghost &ghost, std::chrono::milliseconds deltaTime);
  bool canMove(const TilePosition& tilePosition, const EntityDirection& direction) const;
  RealPosition moveTowards(const RealPosition& realPosition, const EntityDirection& direction,
                           const float distance) const;
  TilePosition moveTowardsGrid(const TilePosition &tilePosition,
      const EntityDirection& direction, const int distance) const;
  bool checkGameOver() const;

private:
  // Game related attributes
  const int m_gameId;
  std::atomic<GameStatus> m_gameStatus = GameStatus::WAITING;
  std::unique_ptr<GameObjects::Level> m_level = nullptr;
  std::thread m_gameThread;

  // Utils
  mutable Utils::Logger m_logger =
      Utils::Logger("GameRunner", Utils::LogLevel::DEBUG);
};

} // namespace GameLogic
} // namespace PacMan

#endif // GAMERUNNER_H
