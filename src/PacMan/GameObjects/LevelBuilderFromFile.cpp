//
// Created by jakubszwedowicz on 2/28/25.
//

#include "LevelBuilderFromFile.h"
#include <fstream>
#include <memory>
#include <type_traits>

#include "Food.h"
#include "Ghost.h"
#include "PacMan.h"
#include "SuperFood.h"
#include "Wall.h"

namespace PacMan {
namespace GameObjects {

LevelBuilderFromFile::LevelBuilderFromFile(const std::string &boardPath)
    : m_boardPath(boardPath) {
  auto board = createBoard();
  if (board) {
    ILevelBuilder::setBoard(std::move(board));
  }
}

std::unique_ptr<Level::Board_t> LevelBuilderFromFile::createBoard() const {
  std::ifstream file(m_boardPath);
  if (!file.is_open()) {
    return nullptr;
  }

  std::unique_ptr<Level::Board_t> board = std::make_unique<Level::Board_t>();
  for (std::string line; std::getline(file, line);) {
    Level::Board_t::value_type row;
    for (unsigned int i = 0; i < line.size(); i++) {
      std::unique_ptr<Entities::IEntity> entity;
      switch (line[i]) {
      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::EMPTY):
        entity = nullptr;
        break;

      case static_cast<std::underlying_type_t<Entities::EntityType>>(
          Entities::EntityType::BRIDGE):
        //            entity = std::make_unique<Entities::Bridge>();
        entity = nullptr;
        // TODO: Log error
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
      }
      row.push_back(std::move(entity));
    }
    board->push_back(std::move(row));
  }

  return board;
}

} // namespace GameObjects
} // namespace PacMan