#include "client/screens/GameScreen.hpp"
#include "client/screen/ScreenManager.hpp"
#include "client/screens/MenuScreen.hpp"
#include "core/ecs/Components.hpp"
#include "core/maps/MapsManager.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <imgui.h>

namespace pacman::client::screens {

GameScreen::GameScreen(screen::ScreenManager &screenManager,
                       std::string mapPath)
    : m_screenManager(screenManager), m_mapPath(std::move(mapPath)),
      m_simulation(), m_renderer() {}

void GameScreen::onEnter() {
  LOG_I("GameScreen entered, loading map: {}", m_mapPath);

  auto result = core::maps::MapsManager::loadFromFile(m_mapPath);
  if (!result) {
    LOG_E("Failed to load map: {}", result.error());
    m_screenManager.setScreen(
        std::make_unique<MenuScreen>(m_screenManager, m_mapPath));
    return;
  }

  m_map = std::move(*result);
  spawnEntitiesFromMap();
  LOG_I("GameScreen ready with {} entities",
        m_registry.storage<entt::entity>().size());
}

void GameScreen::onExit() {
  LOG_I("GameScreen exited");
  m_registry.clear();
  m_localPlayer = entt::null;
}

void GameScreen::handleEvent(const sf::Event &event) {
  if (event.is<sf::Event::KeyPressed>()) {
    const auto *key = event.getIf<sf::Event::KeyPressed>();
    if (key && key->code == sf::Keyboard::Key::Escape) {
      m_screenManager.setScreen(
          std::make_unique<MenuScreen>(m_screenManager, m_mapPath));
    }
  }
}

void GameScreen::update(float dt) {
  m_simulation.applyInput(m_registry, m_localPlayer, m_lastInput);
  m_simulation.update(m_registry, dt, m_map);
}

void GameScreen::draw(sf::RenderWindow &window) {
  m_renderer.render(window, m_registry, m_map);

  ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5f);
  ImGui::Begin(
      "HUD", nullptr,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

  if (m_localPlayer != entt::null) {
    auto *ps = m_registry.try_get<core::ecs::PlayerState>(m_localPlayer);
    if (ps) {
      ImGui::Text("Score: %d", ps->score);
      ImGui::Text("Lives: %d", ps->lives);
    }
  }

  ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
  ImGui::End();
}

void GameScreen::setLastInput(const core::ecs::Input &input) { m_lastInput = input; }

void GameScreen::spawnEntitiesFromMap() {
  float ts = m_map.tileSize;

  for (int row = 0; row < static_cast<int>(m_map.height); ++row) {
    for (int col = 0; col < static_cast<int>(m_map.width); ++col) {
      char tile = m_map.tileAt(col, row);
      float x = static_cast<float>(col) * ts;
      float y = static_cast<float>(row) * ts;

      if (tile == '#') {
        auto wall = m_registry.create();
        m_registry.emplace<core::ecs::Position>(wall, x, y);
        m_registry.emplace<core::ecs::Collider>(wall, ts, ts);
        m_registry.emplace<core::ecs::WallTag>(wall);
      } else if (tile == '.') {
        auto pellet = m_registry.create();
        m_registry.emplace<core::ecs::Position>(pellet, x, y);
        m_registry.emplace<core::ecs::PelletTag>(pellet);
      } else if (tile == 'o') {
        auto powerPellet = m_registry.create();
        m_registry.emplace<core::ecs::Position>(powerPellet, x, y);
        m_registry.emplace<core::ecs::PowerPelletTag>(powerPellet);
      }
    }
  }

  if (!m_map.pacmanSpawns.empty()) {
    const auto &spawn = m_map.pacmanSpawns[0];
    float x = static_cast<float>(spawn.col()) * ts;
    float y = static_cast<float>(spawn.row()) * ts;

    m_localPlayer = m_registry.create();
    m_registry.emplace<core::ecs::Position>(m_localPlayer, x, y);
    m_registry.emplace<core::ecs::Velocity>(m_localPlayer, core::defaultSpeed);
    m_registry.emplace<core::ecs::DirectionState>(m_localPlayer);
    m_registry.emplace<core::ecs::Collider>(m_localPlayer, ts * 0.9f,
                                            ts * 0.9f);
    m_registry.emplace<core::ecs::PlayerState>(m_localPlayer);
    m_registry.emplace<core::ecs::PacManTag>(m_localPlayer);
  }
}

} // namespace pacman::client::screens
