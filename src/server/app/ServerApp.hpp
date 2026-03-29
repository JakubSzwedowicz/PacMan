#pragma once

#include <Utils/Logging/LoggerSubscribed.h>
#include <Utils/PublishSubscribe/IPublisherSubscriber.h>
#include <Utils/Runnables/IRunnable.h>

#include <atomic>
#include <chrono>
#include <memory>

#include "core/Common.hpp"
#include "server/Config.hpp"
#include "server/game/GameRunner.hpp"
#include "server/network/ServerNetwork.hpp"

namespace pacman::server::app {

// Settings derived from ServerConfig that the game loop reads every tick.
// Recomputed in onUpdate() whenever the config changes.
// In a future multithreaded phase, wrap in std::atomic<std::shared_ptr<const LoopSettings>>.
struct LoopSettings {
    float tickDt = core::tickDt;
    std::chrono::nanoseconds tickDuration{static_cast<long long>(core::tickDt * 1'000'000'000LL)};
};

class ServerApp : public Utils::PublishSubscribe::ISubscriber<std::shared_ptr<const ServerConfig>> {
   public:
    void onUpdate(const std::shared_ptr<const ServerConfig> &config) override;

    // Full server lifecycle: signal setup → init → run.
    int main(int argc, char *argv[]);

    // Called by the OS signal handler to request a clean shutdown.
    void stop();

    [[nodiscard]] network::ServerNetwork &network() { return m_network; }
    [[nodiscard]] const ServerConfig &config() const { return *m_config; }

   private:
    void init(int argc, char *argv[]);
    void run();

    // run() helpers — each covers one logical concern per loop iteration
    using Clock = std::chrono::steady_clock;
    [[nodiscard]] float advanceClock(Clock::time_point &last, Clock::time_point now) const;
    void pumpConfigAndNetwork();
    void stepSimulation(float &accumulator);
    void sleepRemainder(Clock::time_point frameStart) const;

    std::shared_ptr<const ServerConfig> m_config;
    LoopSettings m_loopSettings;
    std::unique_ptr<Utils::Runnables::IRunnable> m_configManager;
    network::ServerNetwork m_network;

    std::unique_ptr<game::GameRunner> m_gameRunner;

    core::Tick m_tick = 0;
    std::atomic<bool> m_running{false};

    Utils::Logging::LoggerSubscribed m_logger{"ServerApp"};
};

}  // namespace pacman::server::app
