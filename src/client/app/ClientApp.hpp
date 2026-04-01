#pragma once

#include <Utils/Logging/Logger.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>
#include <Utils/Runnables/IRunnable.h>

#include <memory>
#include <optional>

#include "client/Config.hpp"
#include "client/ScreenManagement/ScreenManager.hpp"
#include "client/ScreenManagement/screens/ScreenFactory.hpp"
#include "client/app/ProcessSpawner.hpp"
#include "client/graphics/AppWindow.hpp"
#include "client/network/ClientNetwork.hpp"

namespace pacman::client {

class ClientApp : public Utils::PublishSubscribe::ISubscriber<std::shared_ptr<const ClientConfig>> {
   public:
    void onUpdate(const std::shared_ptr<const ClientConfig>& config) override { m_config = config; }
    int main(int argc, char* argv[]);

   private:
    void init(int argc, char* argv[]);
    void run();

    std::shared_ptr<const ClientConfig> m_config;
    std::unique_ptr<Utils::Runnables::IRunnable> m_configManager;
    std::optional<graphics::AppWindow> m_appWindow;

    network::ClientNetwork m_network;
    ProcessSpawner m_spawner;
    screen::ScreenManager m_screenManager{[this](screen::ScreenRequest request) {
        return screens::createScreen(std::move(request), m_network, m_spawner);
    }};
    Utils::Logging::Logger m_logger{"ClientApp"};
};

}  // namespace pacman::client
