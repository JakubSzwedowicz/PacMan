//
// Created by jakubszwedowicz on 2/28/25.
//

#include <fstream>
#include <memory>
#include <algorithm>

#include "Entities/Empty.h"
#include "Entities/Food.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "Entities/SuperFood.h"
#include "Entities/Wall.h"
#include "GameEventsManager/GameEventsManager.h"
#include "GameObjects/LevelBuilderFromFile.h"
#include "Strategies/GhostStrategies.h"
#include "Utils/Logger.h"

#include <queue>

namespace PacMan {
namespace GameObjects {

LevelBuilderFromFile::LevelBuilderFromFile(
    const std::string &boardPath,
    GameEvents::GameEventsManager &gameEventsManager)
    : ILevelBuilder(gameEventsManager),
      m_logger(std::make_unique<Utils::Logger>("LevelBuilderFromFile", Utils::LogLevel::DEBUG)),
    m_boardPath(boardPath) {
  createLevel();
  if (!m_level) {
    m_logger->logError("Failed to create board");
  }
}

void LevelBuilderFromFile::createLevel() {
  std::ifstream file(m_boardPath);
  if (!file.is_open()) {
    m_logger->logError("Failed to open file: " + m_boardPath);
    return;
  }
  m_logger->logDebug("Opened file: " + m_boardPath);

  auto board = std::make_unique<Level::Board_t>();
  std::deque<Entities::GhostType> ghostTypesOrdered = {Entities::GhostType::PINKY, Entities::GhostType::BLINKY,  Entities::GhostType::INKY, Entities::GhostType::CLYDE};
  std::vector<Entities::GhostBuilder> ghostBuilders;
  std::vector<Entities::PacManBuilder> pacMansBuilders;

  Entities::TilePosition position = {0, 0};
  for (std::string line; std::getline(file, line); position.y++) {
    position.x = 0;
    Level::Board_t::value_type row;
    for (unsigned int colIdx = 0; colIdx < line.size();
         colIdx++, position.x++) {
      std::unique_ptr<Entities::Entity> entity;
      Entities::EntityType entityType;
      switch (line[colIdx]) {
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::BRIDGE):
        // entity = std::make_unique<Entities::Bridge>();
        m_logger->logError("Bridge entity is not implemented yet!");
        return;
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::EMPTY):
        entityType = Entities::EntityType::EMPTY;
        break;
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::PELLET):
        entityType = Entities::EntityType::PELLET;
        break;
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::GHOST):
        entityType = Entities::EntityType::EMPTY;
        ghostBuilders.push_back({ghostTypesOrdered.front(), position, nullptr, m_gameEventsManager});
        ghostTypesOrdered.pop_front();
        break;
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::PAC_MAN):
        entityType = Entities::EntityType::EMPTY;
        pacMansBuilders.push_back({position, nullptr, m_gameEventsManager});
        break;
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::SUPER_PELLET):
        entityType = Entities::EntityType::SUPER_PELLET;
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::WALL):
        entityType = Entities::EntityType::WALL;
        break;
      default:;
        m_logger->logError("Unknown entity of char '" +
                          std::string({line[colIdx]}) + "'" +
                          " at position (col=" + std::to_string(colIdx) +
                          ", row=" + std::to_string(board->size()) + ")");
        break;
      }

      row.push_back(entityType);
    }
    board->push_back(std::move(row));
  }

  m_logger->logInfo("Created a board from file: " + m_boardPath +
                   ", of size: " + std::to_string(board->size()) + "x" +
                   std::to_string(board->front().size()));
  m_level = std::make_unique<Level>(std::move(*board));

  Level::Ghosts_t ghosts;
  for (auto& ghostBuilder : ghostBuilders) {
    ghostBuilder.level = m_level.get();
    ghosts.emplace_back(ghostBuilder.build());
  }
  m_level->setGhosts(std::move(ghosts));

  Level::Pacmans_t pacMans;
  for (auto& pacManBuilder : pacMansBuilders) {
    pacManBuilder.level = m_level.get();
    pacMans.emplace_back(pacManBuilder.build());
  }
  m_level->setPacMans(std::move(pacMans));
}

} // namespace GameObjects
} // namespace PacMan