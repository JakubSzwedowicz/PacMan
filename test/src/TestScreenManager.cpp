#include "client/screen/Screen.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/screen/ScreenRequest.hpp"

#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <string>

using pacman::client::screen::NoScreenRequest;
using pacman::client::screen::OpenMenuRequest;
using pacman::client::screen::Screen;
using pacman::client::screen::ScreenManager;
using pacman::client::screen::ScreenRequest;

class MockScreen : public Screen {
   public:
    explicit MockScreen(std::string name) : m_name(std::move(name)) {}

    void onEnter() override { enterCalled = true; }
    void onExit() override { exitCalled = true; }
    void handleEvent(const sf::Event& /*event*/) override {}
    ScreenRequest update(float /*dt*/) override {
        updateCalled = true;
        return takeQueuedRequest();
    }
    void draw(sf::RenderWindow& /*window*/) override {}

    bool enterCalled = false;
    bool exitCalled = false;
    bool updateCalled = false;
    std::string m_name;
};

class ScreenManagerFixture : public ::testing::Test {
   protected:
    std::deque<std::unique_ptr<Screen>> screens;
    ScreenManager manager{[this](ScreenRequest) {
        if (screens.empty()) return std::unique_ptr<Screen>{};
        auto next = std::move(screens.front());
        screens.pop_front();
        return next;
    }};

    MockScreen* enqueue(std::string name) {
        auto screen = std::make_unique<MockScreen>(std::move(name));
        auto* ptr = screen.get();
        screens.push_back(std::move(screen));
        return ptr;
    }
};

TEST_F(ScreenManagerFixture, InitialScreenTransition) {
    auto* ptr = enqueue("first");

    manager.requestScreen(OpenMenuRequest{});

    EXPECT_TRUE(ptr->enterCalled);
    EXPECT_TRUE(manager.hasScreen());
}

TEST_F(ScreenManagerFixture, RequestReplacesCurrentScreen) {
    auto* firstPtr = enqueue("first");
    auto* secondPtr = enqueue("second");

    manager.requestScreen(OpenMenuRequest{});
    EXPECT_TRUE(firstPtr->enterCalled);

    manager.requestScreen(OpenMenuRequest{});

    EXPECT_TRUE(firstPtr->exitCalled);
    EXPECT_TRUE(secondPtr->enterCalled);
    EXPECT_EQ(manager.getCurrentScreen(), secondPtr);
}

TEST_F(ScreenManagerFixture, UpdateDelegatesToCurrentScreen) {
    auto* ptr = enqueue("test");
    manager.requestScreen(OpenMenuRequest{});

    manager.update(0.016f);
    EXPECT_TRUE(ptr->updateCalled);
}

TEST_F(ScreenManagerFixture, NoScreenDoesNotCrash) {
    EXPECT_FALSE(manager.hasScreen());
    EXPECT_NO_THROW(manager.update(0.016f));
}

TEST_F(ScreenManagerFixture, QuitRequestSetsQuitFlag) {
    manager.requestScreen(pacman::client::screen::QuitAppRequest{});
    EXPECT_TRUE(manager.shouldQuit());
}
