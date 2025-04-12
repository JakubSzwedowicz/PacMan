//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <memory>

#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameLogic/GameHandler.h"
#include "GameLogic/GameRunner.h"
#include "GameLogic/GameSession.h"
#include "GameObjects/Level.h"
#include "GameObjects/LevelBuilderFromFile.h"

namespace PacMan {
namespace GameLogic {

GameHandler::GameHandler(GameEvents::GameEventsManager &gameEventsManager, )
    : m_gameEventsManager(gameEventsManager) {}

std::unique_ptr<pacMan::GameLogic::GameSession>
GameHandler::loadGame(const std::string &boardName, int numberOfPlayers) {
  m_logger.logInfo("Starting a game from board '" + boardName + "'");

  GameObjects::LevelBuilderFromFile builder = GameObjects::LevelBuilderFromFile(
      std::string(RESOURCES_DIR) + "/" + boardName, m_gameEventsManager);
  auto level = builder.release();

  if (level == nullptr) {
    m_logger.logError("Failed to load level from file '" + boardName + "'");
    return nullptr;
  }

  m_logger.logInfo("Returning a game with id '" + std::to_string(m_nextGameId) +
                   "'");
  auto ghostStrategies = getGhostStrategies(*level);
  std::for_each(level->getGhosts().begin(), level->getGhosts().end(),
                [&ghostStrategies](const auto &ghost) {
                  ghost->setGhostStrategies(
                      ghostStrategies[ghost->getGhostType()]);
                });

  return std::make_unique<pacMan::GameLogic::GameSession>(
      (m_nextGameId++), std::move(level), m_gameEventsManager, numberOfPlayers);
}

GhostTypeToGhostStateToGhostStrategies_t
GameHandler::getGhostStrategies(const GameObjects::Level &level) const {
  GhostTypeToGhostStateToGhostStrategies_t strategies;
  for (const auto &ghost : level.getGhosts()) {
    auto scatterStrat = std::make_unique<Strategies::ScatterStrategy>();
    auto frightenedStrat = std::make_unique<Strategies::FrightenedStrategy>();
    auto eatenStrat = std::make_unique<Strategies::EatenStrategy>();
    std::unique_ptr<Strategies::IGhostStrategy> chaseStrat = nullptr;
    switch (ghost->getGhostType()) {
    case GhostType::BLINKY:
      chaseStrat = std::make_unique<Strategies::BlinkyChaseStrategy>();
      break;
    case GhostType::CLYDE:
      chaseStrat = std::make_unique<Strategies::ClydeChaseStrategy>();
      break;
    case GhostType::INKY:
      chaseStrat = std::make_unique<Strategies::InkyChaseStrategy>();
      break;
    case GhostType::PINKY:
      chaseStrat = std::make_unique<Strategies::PinkyChaseStrategy>();
      break;
    default:
      m_logger.logError("Unknown ghost type!" +
                        toString(ghost->getGhostType()));
      chaseStrat = std::make_unique<Strategies::BlinkyChaseStrategy>();
      break;
    }
    GhostStateToGhostStrategies_t ghostStrategies;
    ghostStrategies.emplace(GhostState::SCATTERING, std::move(scatterStrat));
    ghostStrategies.emplace(GhostState::FRIGHTENED, std::move(frightenedStrat));
    ghostStrategies.emplace(GhostState::EATEN, std::move(eatenStrat));
    ghostStrategies.emplace(GhostState::CHASING, std::move(chaseStrat));
    strategies.emplace(ghost->getGhostType(), std::move(ghostStrategies));
  }

  return strategies;
}

} // namespace GameLogic
} // namespace PacMan