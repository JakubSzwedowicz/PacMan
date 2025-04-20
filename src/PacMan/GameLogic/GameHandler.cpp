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

GameHandler::GameHandler(GameEvents::GameEventsManager &gameEventsManager,
                         bool isClient)
    : m_gameEventsManager(gameEventsManager), m_isClient(isClient) {}

std::unique_ptr<GameLogic::GameSession>
GameHandler::loadGame(const std::string &boardName, const int gameSessionId,
                      const int numberOfPlayers) {
  m_logger.logInfo("Starting a game from board '" + boardName + "'");

  GameObjects::LevelBuilderFromFile builder = GameObjects::LevelBuilderFromFile(
      std::string(RESOURCES_DIR) + "/" + boardName, m_gameEventsManager);
  auto level = builder.release();

  if (level == nullptr) {
    m_logger.logError("Failed to load level from file '" + boardName + "'");
    return nullptr;
  }

  ensureNumberOfPlayers(*level, numberOfPlayers);

  auto ghostStrategies = getGhostStrategies(*level);
  std::for_each(level->getGhosts().begin(), level->getGhosts().end(),
                [&ghostStrategies](const auto &ghost) {
                  ghost->setGhostStrategies(
                      ghostStrategies[ghost->getGhostType()]);
                });

  m_logger.logInfo("Returning a game with id '" +
                   std::to_string(gameSessionId) + "'");
  return std::make_unique<GameLogic::GameSession>(
      (gameSessionId), m_isClient, std::move(level), m_gameEventsManager);
}

int GameHandler::getUniqueGameSessionId() { return (m_nextGameSessionId++); }

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

void GameHandler::ensureNumberOfPlayers(Level &level,
                                        const int numberOfPlayers) const {

  // Ensure number of players match:
  if (numberOfPlayers != level.getPacMans().size()) {
    m_logger.logWarning("Requested number of players " +
                        std::to_string(numberOfPlayers) +
                        " doesn't match board default " +
                        std::to_string(level.getPacMans().size()) + "!");

    auto pacMans = std::move(level.getPacMans());
    if (numberOfPlayers < level.getPacMans().size()) {
      m_logger.logWarning(
          "Removing " +
          std::to_string(level.getPacMans().size() - numberOfPlayers) +
          " players!");
      pacMans.resize(numberOfPlayers);
    } else {
      m_logger.logWarning(
          "Adding " +
          std::to_string(numberOfPlayers - level.getPacMans().size()) +
          " players!");
      const size_t maxPacManIdxToCopy = pacMans.size();
      for (size_t i = pacMans.size(); i < numberOfPlayers; i++) {
        auto &pacManToCopy = pacMans[std::rand() % maxPacManIdxToCopy];
        PacManBuilder builder = {pacManToCopy->getStartingPosition(), &level,
                                 m_gameEventsManager};
        pacMans.emplace_back(builder.build());
      }
    }
    level.setPacMans(std::move(pacMans));
  }
}

} // namespace GameLogic
} // namespace PacMan