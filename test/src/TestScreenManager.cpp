#include "client/screen/Screen.hpp"
#include "client/screen/ScreenManager.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace pacman::client;

class MockScreen : public Screen {
public:
  explicit MockScreen(std::string name) : m_name(std::move(name)) {}

  void onEnter() override { enterCalled = true; }
  void onExit() override { exitCalled = true; }
  void handleEvent(const sf::Event & /*event*/) override {}
  void update(float /*dt*/) override { updateCalled = true; }
  void draw(sf::RenderWindow & /*window*/) override {}

  bool enterCalled = false;
  bool exitCalled = false;
  bool updateCalled = false;
  std::string m_name;
};

class ScreenManagerFixture : public ::testing::Test {
protected:
  ScreenManager manager;
};

TEST_F(ScreenManagerFixture, InitialScreenTransition) {
  auto screen = std::make_unique<MockScreen>("first");
  auto *ptr = screen.get();

  manager.setScreen(std::move(screen));
  EXPECT_FALSE(ptr->enterCalled);

  manager.applyPendingTransition();
  EXPECT_TRUE(ptr->enterCalled);
  EXPECT_TRUE(manager.hasScreen());
}

TEST_F(ScreenManagerFixture, DeferredTransition) {
  auto first = std::make_unique<MockScreen>("first");
  auto *firstPtr = first.get();
  manager.setScreen(std::move(first));
  manager.applyPendingTransition();
  EXPECT_TRUE(firstPtr->enterCalled);

  auto second = std::make_unique<MockScreen>("second");
  auto *secondPtr = second.get();
  manager.setScreen(std::move(second));

  EXPECT_FALSE(firstPtr->exitCalled);
  EXPECT_FALSE(secondPtr->enterCalled);

  manager.applyPendingTransition();
  EXPECT_TRUE(firstPtr->exitCalled);
  EXPECT_TRUE(secondPtr->enterCalled);
}

TEST_F(ScreenManagerFixture, DoubleTransitionSameFrame) {
  auto first = std::make_unique<MockScreen>("first");
  auto second = std::make_unique<MockScreen>("second");
  auto *secondPtr = second.get();

  manager.setScreen(std::move(first));
  manager.setScreen(std::move(second));

  manager.applyPendingTransition();
  EXPECT_TRUE(secondPtr->enterCalled);
  EXPECT_EQ(manager.getCurrentScreen(), secondPtr);
}

TEST_F(ScreenManagerFixture, UpdateDelegatesToCurrentScreen) {
  auto screen = std::make_unique<MockScreen>("test");
  auto *ptr = screen.get();
  manager.setScreen(std::move(screen));
  manager.applyPendingTransition();

  manager.update(0.016f);
  EXPECT_TRUE(ptr->updateCalled);
}

TEST_F(ScreenManagerFixture, NoScreenDoesNotCrash) {
  EXPECT_FALSE(manager.hasScreen());
  EXPECT_NO_THROW(manager.update(0.016f));
}
