#include "server/phases/AuthoritativeLogic.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <vector>

#include "core/ecs/Components.hpp"

namespace pacman::server::phases {

AuthoritativeLogic::AuthoritativeLogic() {}

RuleEvents AuthoritativeLogic::applyRules(entt::registry& registry, const core::maps::Map& map) {
    RuleEvents events;
    checkPelletPickup(registry, map);
    events.powerPelletEaten = checkPowerPellet(registry, map);
    checkGhostCollision(registry, map);
    return events;
}

void AuthoritativeLogic::checkPelletPickup(entt::registry& registry, const core::maps::Map& map) {
    const float ts = map.tileSize;

    const auto pacView = registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>();
    const auto pelletView = registry.view<const core::ecs::Position, const core::ecs::PelletTag>();

    std::vector<entt::entity> toDestroy;

    for (const auto& pac : pacView) {
        const auto& pacPos = pacView.get<const core::ecs::Position>(pac);
        auto& state = pacView.get<core::ecs::PlayerState>(pac);

        const auto pacCol = static_cast<core::maps::Tile::Unit>(pacPos.x / ts);
        const auto pacRow = static_cast<core::maps::Tile::Unit>(pacPos.y / ts);

        for (const auto& pellet : pelletView) {
            const auto& pelletPos = pelletView.get<const core::ecs::Position>(pellet);
            const auto pc = static_cast<core::maps::Tile::Unit>(pelletPos.x / ts);
            const auto pr = static_cast<core::maps::Tile::Unit>(pelletPos.y / ts);

            if (pc == pacCol && pr == pacRow) {
                state.score += 10;
                toDestroy.push_back(pellet);
                LOG_D("Player ate pellet at ({},{}), score={}", pc, pr, state.score);
                break;
            }
        }
    }

    for (const auto& e : toDestroy) {
        registry.destroy(e);
    }
}

bool AuthoritativeLogic::checkPowerPellet(entt::registry& registry, const core::maps::Map& map) {
    const float ts = map.tileSize;

    const auto pacView = registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>();
    const auto ppView = registry.view<const core::ecs::Position, const core::ecs::PowerPelletTag>();

    std::vector<entt::entity> toDestroy;
    bool eaten = false;

    for (const auto& pac : pacView) {
        const auto& pacPos = pacView.get<const core::ecs::Position>(pac);
        auto& state = pacView.get<core::ecs::PlayerState>(pac);

        const auto pacCol = static_cast<core::maps::Tile::Unit>(pacPos.x / ts);
        const auto pacRow = static_cast<core::maps::Tile::Unit>(pacPos.y / ts);

        for (const auto& pp : ppView) {
            const auto& ppPos = ppView.get<const core::ecs::Position>(pp);
            const auto pc = static_cast<core::maps::Tile::Unit>(ppPos.x / ts);
            const auto pr = static_cast<core::maps::Tile::Unit>(ppPos.y / ts);

            if (pc == pacCol && pr == pacRow) {
                state.score += 50;
                state.isPowered = true;
                toDestroy.push_back(pp);
                eaten = true;
                LOG_I("Player ate power pellet at ({},{})", pc, pr);
            }
        }
    }

    for (const auto& e : toDestroy) {
        registry.destroy(e);
    }
    return eaten;
}

void AuthoritativeLogic::checkGhostCollision(entt::registry& registry, const core::maps::Map& map) {
    const float ts = map.tileSize;

    const auto pacView = registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>();
    const auto ghostView = registry.view<const core::ecs::Position, core::ecs::GhostState, const core::ecs::GhostTag>();

    for (const auto& pac : pacView) {
        const auto& pacPos = pacView.get<const core::ecs::Position>(pac);
        auto& state = pacView.get<core::ecs::PlayerState>(pac);
        if (state.lives <= 0) continue;

        const auto pacCol = static_cast<core::maps::Tile::Unit>(pacPos.x / ts);
        const auto pacRow = static_cast<core::maps::Tile::Unit>(pacPos.y / ts);

        for (const auto& ghost : ghostView) {
            const auto& ghostPos = ghostView.get<const core::ecs::Position>(ghost);
            auto& ghostState = ghostView.get<core::ecs::GhostState>(ghost);

            const auto gc = static_cast<core::maps::Tile::Unit>(ghostPos.x / ts);
            const auto gr = static_cast<core::maps::Tile::Unit>(ghostPos.y / ts);

            if (gc != pacCol || gr != pacRow) continue;

            if (ghostState.mode == core::ecs::GhostState::Mode::Frightened) {
                state.score += 200;
                ghostState.mode = core::ecs::GhostState::Mode::Eaten;
                LOG_I("Player ate ghost, score={}", state.score);
            } else if (ghostState.mode != core::ecs::GhostState::Mode::Eaten) {
                state.lives--;
                state.isPowered = false;
                LOG_I("Ghost caught player, lives={}", state.lives);
            }
        }
    }
}

}  // namespace pacman::server::phases
