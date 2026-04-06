#include "core/maps/MapsManager.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

using namespace pacman::core;
using namespace pacman::core::maps;

class MapsManagerFixture : public ::testing::Test {
protected:
  // width/height are intentionally absent — they are derived from tiles in loadFromJson.
  static constexpr const char *validJson = R"({
    "name": "Test",
    "tileSize": 16.0,
    "tiles": ["###", "#.#", "###"],
    "pacmanSpawns": [{"pos": [1, 1]}],
    "ghostSpawns": {
      "blinky": {"pos": [0, 0]},
      "pinky": {"pos": [0, 0]},
      "inky": {"pos": [0, 0]},
      "clyde": {"pos": [0, 0]}
    }
  })";

  // Row 1 has a different length than the rest — triggers the row-width check.
  static constexpr const char *widthMismatchJson = R"({
    "name": "Bad",
    "tileSize": 16.0,
    "tiles": ["###", "#.####", "###"],
    "pacmanSpawns": [],
    "ghostSpawns": {
      "blinky": {"pos": [0, 0]},
      "pinky": {"pos": [0, 0]},
      "inky": {"pos": [0, 0]},
      "clyde": {"pos": [0, 0]}
    }
  })";

  // Empty tiles array — triggers the "Map dimensions must be positive" check.
  static constexpr const char *heightMismatchJson = R"({
    "name": "Bad",
    "tileSize": 16.0,
    "tiles": [],
    "pacmanSpawns": [],
    "ghostSpawns": {
      "blinky": {"pos": [0, 0]},
      "pinky": {"pos": [0, 0]},
      "inky": {"pos": [0, 0]},
      "clyde": {"pos": [0, 0]}
    }
  })";

  static constexpr const char *outOfBoundsSpawnJson = R"({
    "name": "Bad",
    "tileSize": 16.0,
    "tiles": ["###", "#.#", "###"],
    "pacmanSpawns": [{"pos": [10, 10]}],
    "ghostSpawns": {
      "blinky": {"pos": [0, 0]},
      "pinky": {"pos": [0, 0]},
      "inky": {"pos": [0, 0]},
      "clyde": {"pos": [0, 0]}
    }
  })";
};

TEST_F(MapsManagerFixture, LoadFromJsonHappyPath) {
  auto result = MapsManager::loadFromJson(validJson);
  ASSERT_TRUE(result.has_value());

  const auto &map = *result;
  EXPECT_EQ(map.name, "Test");
  EXPECT_EQ(map.width, 3u);
  EXPECT_EQ(map.height, 3u);
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
  EXPECT_NE(result.error().find("positive"), std::string::npos);
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
  EXPECT_EQ(map.tileAt(2, 0), '#');
  EXPECT_EQ(map.tileAt(0, 2), '#');
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
