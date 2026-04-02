#pragma once

#include <Utils/Logging/Logger.h>

#include <string>

#include "client/network/ClientNetwork.hpp"
#include "client/ScreenManagement/Screen.hpp"
#include "core/Common.hpp"
#include "core/protocol/Packets.hpp"

namespace pacman::client::screens {

// Shows the end-of-round ranking table with a [Back to Menu] button.
// This screen is fully static — no network events expected after RoundEnd.
class ResultsScreen : public screen::Screen {
   public:
    ResultsScreen(network::ClientNetwork *network,  // nullptr if offline
                  core::protocol::RoundEndPacket results);

    // Screen
    void onEnter() override;
    void onExit() override;
    screen::ScreenRequest update(float dt, const input::InputSnapshot &input) override;
    void draw(sf::RenderWindow &window) override;

   private:
    network::ClientNetwork *m_network;  // non-owning, may be nullptr
    core::protocol::RoundEndPacket m_results;

    Utils::Logging::Logger m_logger{"ResultsScreen"};
};

}  // namespace pacman::client::screens
