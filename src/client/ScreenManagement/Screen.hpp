#pragma once

#include <Utils/PublishSubscribe/IPublisherSubscriber.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

#include "client/Config.hpp"
#include "client/input/InputSnapshot.hpp"
#include "client/ScreenManagement/ScreenRequest.hpp"

namespace pacman::client::screen {

class Screen : public Utils::PublishSubscribe::ISubscriber<std::shared_ptr<const ClientConfig>> {
   public:
    Screen() { ISubscriber<std::shared_ptr<const ClientConfig>>::pull(); }
    virtual ~Screen() = default;

    void onUpdate(const std::shared_ptr<const ClientConfig>& config) override { m_config = config; }

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual ScreenRequest update(float dt, const input::InputSnapshot& input) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

   protected:
    [[nodiscard]] const ClientConfig& config() const { return *m_config; }
    [[nodiscard]] std::shared_ptr<const ClientConfig> configPtr() const { return m_config; }

    void queueRequest(ScreenRequest request) { m_pendingRequest = std::move(request); }

    [[nodiscard]] ScreenRequest takeQueuedRequest() {
        ScreenRequest request = std::move(m_pendingRequest);
        m_pendingRequest = NoScreenRequest{};
        return request;
    }

   private:
    std::shared_ptr<const ClientConfig> m_config;
    ScreenRequest m_pendingRequest{NoScreenRequest{}};
};

}  // namespace pacman::client::screen
