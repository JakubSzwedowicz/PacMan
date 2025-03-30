//
// Created by jakubszwedowicz on 2/28/25.
//

#include <fstream>
#include <memory>

#include "Entities/Empty.h"
#include "Entities/Food.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "Entities/SuperFood.h"
#include "Entities/Wall.h"
#include "GameObjects/LevelBuilderFromFile.h"
#include "Utils/Logger.h"
#include "GameEventsManager/GameEventsManager.h"

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
  Level::Pacmans_t pacmans;
  Level::Ghosts_t ghosts;
  Entities::TilePosition position = {0, 0};
  for (std::string line; std::getline(file, line); position.y++) {
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
        ghosts.emplace_back(
            std::make_unique<Entities::Ghost>(m_level.get(), m_gameEventsManager));
        break;
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::PAC_MAN):
        entityType = Entities::EntityType::EMPTY;
        pacmans.emplace_back(
            std::make_unique<Entities::PacMan>(m_level.get(), m_gameEventsManager));
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
  m_level = std::make_unique<Level>(std::move(*board), std::move(pacmans),
                                    std::move(ghosts));
}

} // namespace GameObjects
} // namespace PacMan