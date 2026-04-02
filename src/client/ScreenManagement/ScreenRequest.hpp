#pragma once

#include <array>
#include <optional>
#include <unordered_map>
#include <variant>

#include <entt/entt.hpp>

#include "core/Common.hpp"
#include "core/maps/Map.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screen {

struct NoScreenRequest {};
struct QuitAppRequest {};
struct OpenMenuRequest {};

struct OpenLobbyRequest {
    core::PlayerId localPlayerId = 0;
    bool isHost = false;
};

struct OpenLoadingRequest {
    core::protocol::GameStartPacket gameStart;
    core::PlayerId localPlayerId = 0;
    bool isHost = false;
};

struct OpenOfflineGameRequest {};

struct OpenNetworkGameRequest {
    entt::registry registry;
    core::maps::Map map;
    std::unordered_map<core::PlayerId, entt::entity> playerEntities;
    std::array<entt::entity, core::ghostCount> ghostEntities{};
    std::optional<core::protocol::GameSnapshotPacket> initialSnapshot;
    core::PlayerId localPlayerId = 0;
    bool isHost = false;
};

struct OpenResultsRequest {
    core::protocol::RoundEndPacket results;
    core::PlayerId localPlayerId = 0;
    bool isHost = false;
};

using ScreenRequest =
    std::variant<NoScreenRequest, QuitAppRequest, OpenMenuRequest, OpenLobbyRequest, OpenLoadingRequest,
                 OpenOfflineGameRequest, OpenNetworkGameRequest, OpenResultsRequest>;

}  // namespace pacman::client::screen
