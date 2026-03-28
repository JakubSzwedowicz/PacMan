#include "core/maps/MapsManager.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

using namespace pacman::core;

class MapsManagerFixture : public ::testing::Test {
protected:
  static constexpr const char *validJson = R"({
    "name": "Test",
    "width": 3,
    "height": 3,
    "tileSize": 16.0,
    "tiles": ["###", "#.#", "###"],
    "pacmanSpawns": [{"data": [1, 1]}],
    "ghostSpawns": {
      "blinky": {"data": [1, 1]},
      "pinky": {"data": [0, 0]},
      "inky": {"data": [0, 0]},
      "clyde": {"data": [0, 0]}
    }
  })";

  static constexpr const char *widthMismatchJson = R"({
    "name": "Bad",
    "width": 5,
    "height": 3,
    "tileSize": 16.0,
    "tiles": ["###", "#.#", "###"],
    "pacmanSpawns": [],
    "ghostSpawns": {
      "blinky": {"data": [0, 0]},
      "pinky": {"data": [0, 0]},
      "inky": {"data": [0, 0]},
      "clyde": {"data": [0, 0]}
    }
  })";

  static constexpr const char *heightMismatchJson = R"({
    "name": "Bad",
    "width": 3,
    "height": 5,
    "tileSize": 16.0,
    "tiles": ["###", "#.#", "###"],
    "pacmanSpawns": [],
    "ghostSpawns": {
      "blinky": {"data": [0, 0]},
      "pinky": {"data": [0, 0]},
      "inky": {"data": [0, 0]},
      "clyde": {"data": [0, 0]}
    }
  })";

  static constexpr const char *outOfBoundsSpawnJson = R"({
    "name": "Bad",
    "width": 3,
    "height": 3,
    "tileSize": 16.0,
    "tiles": ["###", "#.#", "###"],
    "pacmanSpawns": [{"data": [10, 10]}],
    "ghostSpawns": {
      "blinky": {"data": [0, 0]},
      "pinky": {"data": [0, 0]},
      "inky": {"data": [0, 0]},
      "clyde": {"data": [0, 0]}
    }
  })";
};

TEST_F(MapsManagerFixture, LoadFromJsonHappyPath) {
  auto result = MapsManager::loadFromJson(validJson);
  ASSERT_TRUE(result.has_value());

  const auto &map = *result;
  EXPECT_EQ(map.name, "Test");
  EXPECT_EQ(map.width, 3);
  EXPECT_EQ(map.height, 3);
  EXPECT_FLOAT_EQ(map.tileSize, 16.0f);
  EXPECT_EQ(map.tiles.size(), 3u);
  EXPECT_EQ(map.pacmanSpawns.size(), 1u);
  EXPECT_EQ(map.pacmanSpawns[0].col(), 1u);
  EXPECT_EQ(map.pacmanSpawns[0].row(), 1u);
}

TEST_F(MapsManagerFixture, ValidateWidthMismatch) {
  auto result = MapsManager::loadFromJson(widthMismatchJson);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("width"), std::string::npos);
}

TEST_F(MapsManagerFixture, ValidateHeightMismatch) {
  auto result = MapsManager::loadFromJson(heightMismatchJson);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("height"), std::string::npos);
}

TEST_F(MapsManagerFixture, ValidateOutOfBoundsSpawns) {
  auto result = MapsManager::loadFromJson(outOfBoundsSpawnJson);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("out of bounds"), std::string::npos);
}

TEST_F(MapsManagerFixture, TileAtHelper) {
  auto result = MapsManager::loadFromJson(validJson);
  ASSERT_TRUE(result.has_value());

  const auto &map = *result;
  EXPECT_EQ(map.tileAt(0, 0), '#');
  EXPECT_EQ(map.tileAt(1, 1), '.');
  EXPECT_EQ(map.tileAt(-1, 0), '#');
  EXPECT_EQ(map.tileAt(0, 10), '#');
}

TEST_F(MapsManagerFixture, LoadFromFile) {
  std::filesystem::path testDir = std::filesystem::temp_directory_path();
  std::filesystem::path testFile = testDir / "test_map.json";

  {
    std::ofstream f(testFile);
    f << validJson;
  }

  auto result = MapsManager::loadFromFile(testFile);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->name, "Test");

  std::filesystem::remove(testFile);
}

TEST_F(MapsManagerFixture, LoadFromFileMissing) {
  auto result = MapsManager::loadFromFile("/nonexistent/path/map.json");
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("Failed to open"), std::string::npos);
}
