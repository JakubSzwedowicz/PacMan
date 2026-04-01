#pragma once

#include <memory>

#include "client/app/ProcessSpawner.hpp"
#include "client/network/ClientNetwork.hpp"
#include "client/ScreenManagement/Screen.hpp"
#include "client/ScreenManagement/ScreenRequest.hpp"

namespace pacman::client::screens {

std::unique_ptr<screen::Screen> createScreen(screen::ScreenRequest request, network::ClientNetwork &network,
                                              ProcessSpawner &spawner);

}  // namespace pacman::client::screens
