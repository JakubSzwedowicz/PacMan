#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <entt/entt.hpp>
#include <string>
#include <unordered_map>

#include "client/graphics/Renderer.hpp"
#include "client/network/ClientNetwork.hpp"
#include "client/network/ClientNetworkEvents.hpp"
#include "client/ScreenManagement/Screen.hpp"
#include "core/Common.hpp"
#include "core/maps/Map.hpp"
#include "core/simulation/Simulation.hpp"

namespace pacman::client::screens {

class GameScreen : public screen::Screen,
                   public Utils::PublishSubscribe::ISubscriber<network::events::ClientNetworkEvent> {
   public:
    using screen::Screen::onUpdate;

    // Standalone (offline) constructor — loads map from file on onEnter.
    explicit GameScreen(network::ClientNetwork &network);

    // Networked constructor — receives pre-populated registry from LoadingScreen.
    GameScreen(network::ClientNetwork &network, entt::registry &&registry, core::maps::Map map,
               std::unordered_map<core::PlayerId, entt::entity> playerEntities,
               std::array<entt::entity, core::ghostCount> ghostEntities, core::PlayerId localPlayerId, bool isHost);

    void onEnter() override;
    void onExit() override;
    screen::ScreenRequest update(float dt, const input::InputSnapshot &input) override;
    void draw(sf::RenderWindow &window) override;

    // ISubscriber<ClientNetworkEvent>
    void onUpdate(const network::events::ClientNetworkEvent &event) override;

   private:
    void spawnEntitiesFromMap();
    void applySnapshot(const core::protocol::GameSnapshotPacket &snap);

    network::ClientNetwork &m_network;

    entt::registry m_registry;
    core::maps::Map m_map;
    core::simulation::Simulation m_simulation;
    graphics::Renderer m_renderer;

    // Networked mode
    bool m_networked = false;
    core::PlayerId m_localPlayerId = 0;
    bool m_isHost = false;
    std::unordered_map<core::PlayerId, entt::entity> m_playerEntities;
    std::array<entt::entity, core::ghostCount> m_ghostEntities{};

    entt::entity m_localPlayer = entt::null;

    Utils::Logging::Logger m_logger{"GameScreen"};
};

}  // namespace pacman::client::screens
