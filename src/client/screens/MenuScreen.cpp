#include "client/screens/MenuScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include <chrono>
#include <filesystem>
#include <thread>

#include "client/screen/ScreenManager.hpp"
#include "client/screens/LobbyScreen.hpp"

namespace pacman::client::screens {

// Path to the server binary: same directory as the client executable.
static std::string serverBinaryPath() {
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len < 0) return "PacManServer";
    buf[len] = '\0';
    return (std::filesystem::path(buf).parent_path() / "PacManServer").string();
}

MenuScreen::MenuScreen(screen::ScreenManager &screenManager, network::ClientNetwork &network, std::string mapPath,
                       std::string serverAddress, int serverPort)
    : m_screenManager(screenManager),
      m_network(network),
      m_mapPath(std::move(mapPath)),
      m_serverAddress(std::move(serverAddress)),
      m_serverPort(serverPort) {}

void MenuScreen::onEnter() { LOG_I("MenuScreen entered"); }
void MenuScreen::onExit() { LOG_I("MenuScreen exited"); }

void MenuScreen::handleEvent(const sf::Event &event) {
    if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) m_shouldQuit = true;
    }
}

void MenuScreen::update(float /*dt*/) {}

void MenuScreen::draw(sf::RenderWindow & /*window*/) {
    ImGui::SetNextWindowPos(ImVec2(200, 150), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("PacMan", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("PacMan").x) / 2.0f);
    ImGui::Text("PacMan");
    ImGui::Separator();
    ImGui::Spacing();

    float buttonWidth = 200.0f;
    float buttonX = (400 - buttonWidth) / 2.0f;

    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Host Game", ImVec2(buttonWidth, 40))) hostGame();

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Join Game", ImVec2(buttonWidth, 40))) joinGame();

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Quit", ImVec2(buttonWidth, 40))) m_shouldQuit = true;

    ImGui::End();
}

void MenuScreen::hostGame() {
    LOG_I("Hosting game on port {}", m_serverPort);

    auto serverBin = serverBinaryPath();
    std::vector<std::string> args{
        "--port",
        std::to_string(m_serverPort),
        "--map_path",
        m_mapPath,
    };

    if (!m_spawner.spawn(serverBin, args)) {
        LOG_E("Failed to spawn server at '{}'", serverBin);
        return;
    }

    // Give the server a moment to bind its port before we connect.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (!m_network.connect("127.0.0.1", static_cast<uint16_t>(m_serverPort))) {
        LOG_E("Failed to connect to local server on port {}", m_serverPort);
        m_spawner.kill();
        return;
    }

    LOG_I("Connected to local server");
    m_screenManager.setScreen(std::make_unique<LobbyScreen>(m_screenManager, m_network, m_mapPath, m_serverAddress,
                                                            m_serverPort, 0 /*localPlayerId — filled by GameStart*/,
                                                            true /*isHost*/));
}

void MenuScreen::joinGame() {
    LOG_I("Joining game at {}:{}", m_serverAddress, m_serverPort);

    if (!m_network.connect(m_serverAddress, static_cast<uint16_t>(m_serverPort))) {
        LOG_E("Failed to connect to {}:{}", m_serverAddress, m_serverPort);
        return;
    }

    LOG_I("Connected to server");
    m_screenManager.setScreen(std::make_unique<LobbyScreen>(m_screenManager, m_network, m_mapPath, m_serverAddress,
                                                            m_serverPort, 0 /*localPlayerId — filled by GameStart*/,
                                                            false /*isHost*/));
}

bool MenuScreen::shouldQuit() const { return m_shouldQuit; }

}  // namespace pacman::client::screens
