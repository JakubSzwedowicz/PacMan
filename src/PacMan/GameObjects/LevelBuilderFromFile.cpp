//
// Created by jakubszwedowicz on 2/28/25.
//

#include "GameObjects/LevelBuilderFromFile.h"
#include <fstream>
#include <memory>

#include "Entities/Empty.h"
#include "Entities/Food.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "Entities/SuperFood.h"
#include "Entities/Wall.h"

namespace PacMan {
namespace GameObjects {

LevelBuilderFromFile::LevelBuilderFromFile(const std::string &boardPath)
    : m_logger("LevelBuilderFromFile", Utils::LogLevel::INFO),
      m_boardPath(boardPath) {
  auto board = createBoard();
  if (!board) {
    m_logger.logError("Failed to create board");
  }
  setBoard(std::move(board));
}

std::unique_ptr<Level::Board_t> LevelBuilderFromFile::createBoard() const {
  std::ifstream file(m_boardPath);
  if (!file.is_open()) {
    m_logger.logError("Failed to open file: " + m_boardPath);
    return nullptr;
  }
  m_logger.logDebug("Opened file: " + m_boardPath);

  std::unique_ptr<Level::Board_t> board = std::make_unique<Level::Board_t>();
  Entities::Position position = {0, 0};
  for (std::string line; std::getline(file, line); position.y++) {
    Level::Board_t::value_type row;
    for (unsigned int colIdx = 0; colIdx < line.size();
         colIdx++, position.x++) {
      std::unique_ptr<Entities::IEntity> entity;
      switch (line[colIdx]) {
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::EMPTY):
        entity = std::make_unique<Entities::Empty>();
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::BRIDGE):
        // entity = std::make_unique<Entities::Bridge>();
        entity = std::make_unique<Entities::Empty>();
        m_logger.logError("Bridge entity is not implemented yet");
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::FOOD):
        entity = std::make_unique<Entities::Food>();
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::GHOST):
        entity = std::make_unique<Entities::Ghost>();
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::PAC_MAN):
        entity = std::make_unique<Entities::PacMan>();
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::SUPER_FOOD):
        entity = std::make_unique<Entities::SuperFood>();
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::WALL):
        entity = std::make_unique<Entities::Wall>();
        break;
      default:;
        m_logger.logError("Unknown entity of char '" +
                          std::string({line[colIdx]}) + "'" +
                          " at position (col=" + std::to_string(colIdx) +
                          ", row=" + std::to_string(board->size()) + ")");
        break;
      }

      entity->setTilePosition(position);
      row.push_back(std::move(entity));
    }
    board->push_back(std::move(row));
  }

  m_logger.logInfo("Created a board from file: " + m_boardPath +
                   ", of size: " + std::to_string(board->size()) + "x" +
                   std::to_string(board->front().size()));
  return board;
}

} // namespace GameObjects
} // namespace PacMan