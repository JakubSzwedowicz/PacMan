#include "server/phases/AuthoritativeLogic.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <algorithm>
#include <vector>

#include "core/ecs/Components.hpp"

namespace pacman::server::phases {

namespace {
constexpr float hitInvulnerabilitySeconds = 2.0f;
}

AuthoritativeLogic::AuthoritativeLogic() {}

RuleEvents AuthoritativeLogic::applyRules(entt::registry& registry, const core::maps::Map& map, float dt) {
    RuleEvents events;
    updatePlayerTimers(registry, dt);
    checkPelletPickup(registry, map);
    events.powerPelletEaten = checkPowerPellet(registry, map);
    checkGhostCollision(registry, map);
    return events;
}

void AuthoritativeLogic::updatePlayerTimers(entt::registry& registry, float dt) {
    auto view = registry.view<core::ecs::PlayerState, const core::ecs::PacManTag>();
    for (auto player : view) {
        auto& state = view.get<core::ecs::PlayerState>(player);
        if (state.invulnerableSeconds > 0.0f) {
            state.invulnerableSeconds = std::max(0.0f, state.invulnerableSeconds - dt);
        }
    }
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

    const auto pacView = registry.view<core::ecs::Position, core::ecs::PlayerState, core::ecs::DirectionState,
                                       const core::ecs::PacManTag>();
    const auto ghostView = registry.view<const core::ecs::Position, core::ecs::GhostState, const core::ecs::GhostTag>();

    for (const auto& pac : pacView) {
        auto& pacPos = pacView.get<core::ecs::Position>(pac);
        auto& state = pacView.get<core::ecs::PlayerState>(pac);
        auto& direction = pacView.get<core::ecs::DirectionState>(pac);
        if (state.lives <= 0) continue;
        if (state.invulnerableSeconds > 0.0f) continue;

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
                state.invulnerableSeconds = state.lives > 0 ? hitInvulnerabilitySeconds : 0.0f;
                pacPos.x = static_cast<float>(state.spawnTile.col()) * ts;
                pacPos.y = static_cast<float>(state.spawnTile.row()) * ts;
                direction.current = core::ecs::Direction::None;
                direction.next = core::ecs::Direction::None;
                if (state.lives > 0) {
                    LOG_I("Ghost caught player, lives remaining={}", state.lives);
                } else {
                    LOG_I("Ghost caught player — game over");
                }
                break;
            }
        }
    }
}

}  // namespace pacman::server::phases
