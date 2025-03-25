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

class GameRunner {
public:
  GameRunner(int gameId, std::unique_ptr<GameObjects::Level> level);
  bool startGame();

  int getGameId() const { return m_gameId; }

private:
  void gameLoop();
  void update(std::chrono::milliseconds deltaTime);
  void updatePacMan(PacMan::GameObjects::Entities::PacMan &pacman,
                    std::chrono::milliseconds deltaTime,
                    PacMan::GameObjects::Level::Board_t &board);
  void updateGhost(GameObjects::Entities::Ghost &ghost,
                   std::chrono::milliseconds deltaTime,
                   PacMan::GameObjects::Level::Board_t &board);
  bool canMove(GameObjects::Entities::TilePosition tilePosition,
               GameObjects::Entities::EntityDirection direction,
               PacMan::GameObjects::Level::Board_t &board) const;
  GameObjects::Entities::RealPosition
  moveTowards(GameObjects::Entities::RealPosition realPosition,
              GameObjects::Entities::EntityDirection direction,
              float distance) const;
  void moveTowardsGrid(int &x, int &y,
                       GameObjects::Entities::EntityDirection direction,
                       float distance) const;
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
