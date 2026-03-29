#include "server/phases/AuthoritativeLogic.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <vector>

#include "core/ecs/Components.hpp"

namespace pacman::server::phases {

AuthoritativeLogic::AuthoritativeLogic() {}

RuleEvents AuthoritativeLogic::applyRules(entt::registry &registry, const core::maps::Map &map) {
    RuleEvents events;
    checkPelletPickup(registry, map);
    events.powerPelletEaten = checkPowerPellet(registry, map);
    checkGhostCollision(registry, map);
    return events;
}

void AuthoritativeLogic::checkPelletPickup(entt::registry &registry, const core::maps::Map &map) {
    float ts = map.tileSize;

    auto pacView = registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>();
    auto pelletView = registry.view<const core::ecs::Position, const core::ecs::PelletTag>();

    std::vector<entt::entity> toDestroy;

    for (auto pac : pacView) {
        const auto &pacPos = pacView.get<const core::ecs::Position>(pac);
        auto &state = pacView.get<core::ecs::PlayerState>(pac);

        auto pacCol = static_cast<size_t>(pacPos.x / ts);
        auto pacRow = static_cast<size_t>(pacPos.y / ts);

        for (auto pellet : pelletView) {
            const auto &pelletPos = pelletView.get<const core::ecs::Position>(pellet);
            auto pc = static_cast<size_t>(pelletPos.x / ts);
            auto pr = static_cast<size_t>(pelletPos.y / ts);

            if (pc == pacCol && pr == pacRow) {
                state.score += 10;
                toDestroy.push_back(pellet);
                LOG_D("Player ate pellet at ({},{}), score={}", pc, pr, state.score);
            }
        }
    }

    for (auto e : toDestroy) {
        registry.destroy(e);
    }
}

bool AuthoritativeLogic::checkPowerPellet(entt::registry &registry, const core::maps::Map &map) {
    float ts = map.tileSize;

    auto pacView = registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>();
    auto ppView = registry.view<const core::ecs::Position, const core::ecs::PowerPelletTag>();

    std::vector<entt::entity> toDestroy;
    bool eaten = false;

    for (auto pac : pacView) {
        const auto &pacPos = pacView.get<const core::ecs::Position>(pac);
        auto &state = pacView.get<core::ecs::PlayerState>(pac);

        auto pacCol = static_cast<size_t>(pacPos.x / ts);
        auto pacRow = static_cast<size_t>(pacPos.y / ts);

        for (auto pp : ppView) {
            const auto &ppPos = ppView.get<const core::ecs::Position>(pp);
            auto pc = static_cast<size_t>(ppPos.x / ts);
            auto pr = static_cast<size_t>(ppPos.y / ts);

            if (pc == pacCol && pr == pacRow) {
                state.score += 50;
                state.isPowered = true;
                toDestroy.push_back(pp);
                eaten = true;
                LOG_I("Player ate power pellet at ({},{})", pc, pr);
            }
        }
    }

    for (auto e : toDestroy) {
        registry.destroy(e);
    }
    return eaten;
}

void AuthoritativeLogic::checkGhostCollision(entt::registry &registry, const core::maps::Map &map) {
    float ts = map.tileSize;

    auto pacView = registry.view<const core::ecs::Position, core::ecs::PlayerState, const core::ecs::PacManTag>();
    auto ghostView = registry.view<const core::ecs::Position, core::ecs::GhostState, const core::ecs::GhostTag>();

    for (auto pac : pacView) {
        const auto &pacPos = pacView.get<const core::ecs::Position>(pac);
        auto &state = pacView.get<core::ecs::PlayerState>(pac);
        if (state.lives <= 0) continue;

        auto pacCol = static_cast<size_t>(pacPos.x / ts);
        auto pacRow = static_cast<size_t>(pacPos.y / ts);

        for (auto ghost : ghostView) {
            const auto &ghostPos = ghostView.get<const core::ecs::Position>(ghost);
            auto &ghostState = ghostView.get<core::ecs::GhostState>(ghost);

            auto gc = static_cast<size_t>(ghostPos.x / ts);
            auto gr = static_cast<size_t>(ghostPos.y / ts);

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
