//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include "GameLogic/GameRunner.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace GameLogic {

GameRunner::GameRunner(int gameId, std::unique_ptr<GameObjects::Level> level)
    : m_gameId(gameId), m_level(std::move(level)) {}

} // namespace GameLogic
} // namespace PacMan