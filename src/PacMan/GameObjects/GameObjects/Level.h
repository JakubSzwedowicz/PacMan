//
// Created by jakubszwedowicz on 2/28/25.
//

#ifndef LEVEL_H
#define LEVEL_H

#include <memory>
#include <vector>

#include "Entities/Entity.h"
#include "Entities/EntityType.h"
#include "LevelState.h"
#include "Utils/ILogger.h"
#include "Entities/MovingEntity.h"

namespace PacMan {
namespace GameObjects {

namespace Entities {
class PacMan;
class Ghost;
}; // namespace Entities

class Level {
public:
  using Board_t = std::vector<std::vector<Entities::EntityType>>;
  using Pacmans_t = std::vector<std::unique_ptr<Entities::PacMan>>;
  using Ghosts_t = std::vector<std::unique_ptr<Entities::Ghost>>;
  using MovingEntities_t = std::vector<Entities::MovingEntity*>;

  struct GhostScatterPositions {
    Entities::TilePosition blinkyScatterPositionTopRight = {0, 0};
    Entities::TilePosition pinkyScatterPositionTopLeft = {0, 0};
    Entities::TilePosition inkyScatterPositionBottomRight = {0, 0};
    Entities::TilePosition clydeScatterPositionBottomLeft = {0, 0};
  };

  Level(Board_t &&board);

  bool verifyBoard();

  [[nodiscard]] std::vector<Entities::TilePosition>
  getValidAdjacentPositions(const Entities::TilePosition &pos) const;

  [[nodiscard]] bool isReady() const {
    return m_levelState == LevelState::READY;
  }
  [[nodiscard]] bool
  isTilePositionValid(const Entities::TilePosition &pos) const {
    return isTileInsideTheBoard(pos) && m_board[pos.y][pos.x] != Entities::EntityType::WALL;
  }

  [[nodiscard]] bool
  isTileInsideTheBoard(const Entities::TilePosition &pos) const {
    return pos.x >= 0 && static_cast<size_t>(pos.x) < getWidth() &&
           pos.y >= 0 && static_cast<size_t>(pos.y) < getHeight();
  }

  [[nodiscard]] Board_t::value_type::value_type
  getEntityOnTile(const Entities::TilePosition &tilePosition) const {
    return m_board[tilePosition.y][tilePosition.x];
  }
  void setEntityOnTile(const Entities::TilePosition &tilePosition,
                       Entities::EntityType entityType) {
    m_board[tilePosition.y][tilePosition.x] = entityType;
  }

  [[nodiscard]] const Board_t &getBoard() const { return m_board; }
  [[nodiscard]] size_t getWidth() const { return m_board.front().size(); }
  [[nodiscard]] size_t getHeight() const { return m_board.size(); }
  Pacmans_t &getPacMans() { return m_pacMans; }
  Ghosts_t &getGhosts() { return m_ghosts; }
  MovingEntities_t &getMovingEntities() { return m_movingEntities; }
  Entities::Ghost *getGhostOrAnyButNot(const Entities::Ghost &notThisGhost,
                                       Entities::GhostType ghostType) const;
  const Pacmans_t &getPacMans() const { return m_pacMans; }
  const Ghosts_t &getGhosts() const { return m_ghosts; }
  void setPacMans(Pacmans_t &&pacMans);
  void setGhosts(Ghosts_t &&ghosts);
  [[nodiscard]] int getNumberOfFood() const;
  [[nodiscard]] GhostScatterPositions getGhostScatterPositions() const;
  [[nodiscard]] Entities::TilePosition
  getScatteringPositionOfGhost(Entities::GhostType ghostType) const;

private:
  void processSetPacMandAndGhosts();
  Board_t::value_type::value_type
  getMutableEntityOnTile(const Entities::TilePosition &tilePosition) {
    return m_board[tilePosition.y][tilePosition.x];
  }

private:
  mutable std::unique_ptr<Utils::ILogger> m_logger;
  LevelState m_levelState = LevelState::NOT_READY;
  Board_t m_board;
  Pacmans_t m_pacMans;
  Ghosts_t m_ghosts;
  MovingEntities_t m_movingEntities;

  GhostScatterPositions m_ghostScatterPositions;

  int m_numberOfFood = 0;
};

} // namespace GameObjects
} // namespace PacMan

#endif // LEVEL_H
