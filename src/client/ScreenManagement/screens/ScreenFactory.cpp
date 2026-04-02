#include "client/ScreenManagement/screens/ScreenFactory.hpp"

#include "client/app/ProcessSpawner.hpp"

#include "client/ScreenManagement/screens/GameScreen.hpp"
#include "client/ScreenManagement/screens/LoadingScreen.hpp"
#include "client/ScreenManagement/screens/LobbyScreen.hpp"
#include "client/ScreenManagement/screens/MenuScreen.hpp"
#include "client/ScreenManagement/screens/ResultsScreen.hpp"
#include "core/Common.hpp"

namespace pacman::client::screens {

std::unique_ptr<screen::Screen> createScreen(screen::ScreenRequest request, network::ClientNetwork& network,
                                              ProcessSpawner& spawner) {
    return std::visit(
        pacman::overloaded{
            [](const screen::NoScreenRequest&) -> std::unique_ptr<screen::Screen> { return nullptr; },
            [](const screen::QuitAppRequest&) -> std::unique_ptr<screen::Screen> { return nullptr; },
            [&](const screen::OpenMenuRequest&) -> std::unique_ptr<screen::Screen> {
                return std::make_unique<MenuScreen>(network, spawner);
            },
            [&](screen::OpenLobbyRequest& req) -> std::unique_ptr<screen::Screen> {
                return std::make_unique<LobbyScreen>(network, req.localPlayerId, req.isHost);
            },
            [&](screen::OpenLoadingRequest& req) -> std::unique_ptr<screen::Screen> {
                return std::make_unique<LoadingScreen>(network, std::move(req.gameStart), req.localPlayerId,
                                                       req.isHost);
            },
            [&](const screen::OpenOfflineGameRequest&) -> std::unique_ptr<screen::Screen> {
                return std::make_unique<GameScreen>(network);
            },
            [&](screen::OpenNetworkGameRequest& req) -> std::unique_ptr<screen::Screen> {
                return std::make_unique<GameScreen>(network, std::move(req.registry), std::move(req.map),
                                                    std::move(req.playerEntities), req.ghostEntities,
                                                    std::move(req.initialSnapshot), req.localPlayerId, req.isHost);
            },
            [&](screen::OpenResultsRequest& req) -> std::unique_ptr<screen::Screen> {
                return std::make_unique<ResultsScreen>(&network, std::move(req.results));
            },
        },
        request);
}

}  // namespace pacman::client::screens
