#include "client/ScreenManagement/screens/MenuScreen.hpp"

#include <algorithm>
#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>

#include <chrono>
#include <cstring>
#include <filesystem>
#include <thread>

namespace pacman::client::screens {

// Path to the server binary: same directory as the client executable.
static std::string serverBinaryPath() {
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len < 0) return "PacManServer";
    buf[len] = '\0';
    return (std::filesystem::path(buf).parent_path() / "PacManServer").string();
}

MenuScreen::MenuScreen(network::ClientNetwork& network, ProcessSpawner& spawner)
    : m_network(network), m_spawner(spawner) {}

void MenuScreen::onEnter() {
    std::snprintf(m_serverAddressBuffer.data(), m_serverAddressBuffer.size(), "%s", config().serverAddress.get().c_str());
    LOG_I("MenuScreen entered");
}
void MenuScreen::onExit() { LOG_I("MenuScreen exited"); }

screen::ScreenRequest MenuScreen::update(float /*dt*/, const input::InputSnapshot& input) {
    if (input.escapePressed) queueRequest(screen::QuitAppRequest{});
    return takeQueuedRequest();
}

void MenuScreen::draw(sf::RenderWindow& window) {
    const auto size = window.getSize();
    const float panelWidth = std::min(400.0f, std::max(320.0f, static_cast<float>(size.x) - 32.0f));
    const float panelHeight = std::min(300.0f, std::max(260.0f, static_cast<float>(size.y) - 32.0f));

    ImGui::SetNextWindowPos(ImVec2((static_cast<float>(size.x) - panelWidth) * 0.5f,
                                   (static_cast<float>(size.y) - panelHeight) * 0.5f),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
    ImGui::Begin("PacMan", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::SetCursorPosX((panelWidth - ImGui::CalcTextSize("PacMan").x) * 0.5f);
    ImGui::Text("PacMan");
    ImGui::Separator();
    ImGui::Spacing();

    const float buttonWidth = std::min(220.0f, panelWidth - 48.0f);
    const float buttonX = (panelWidth - buttonWidth) * 0.5f;

    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Host Game", ImVec2(buttonWidth, 40))) hostGame();

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Join Game", ImVec2(buttonWidth, 40))) joinGame();

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    ImGui::PushItemWidth(buttonWidth);
    ImGui::InputText("Server IP", m_serverAddressBuffer.data(), m_serverAddressBuffer.size());
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Quit", ImVec2(buttonWidth, 40))) queueRequest(screen::QuitAppRequest{});

    ImGui::End();
}

void MenuScreen::hostGame() {
    LOG_I("Hosting game on port {}", config().serverPort.get());

    auto serverBin = serverBinaryPath();
    std::vector<std::string> args{
        "--port",
        std::to_string(config().serverPort.get()),
        "--mapPath",
        config().mapPath.get(),
    };

    if (!m_spawner.spawn(serverBin, args)) {
        LOG_E("Failed to spawn server at '{}'", serverBin);
        return;
    }

    // Give the server a moment to bind its port before we connect.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (!m_network.connect("127.0.0.1", static_cast<uint16_t>(config().serverPort.get()))) {
        LOG_E("Failed to connect to local server on port {}", config().serverPort.get());
        m_spawner.kill();
        return;
    }

    LOG_I("Connected to local server");
    queueRequest(screen::OpenLobbyRequest{0 /*localPlayerId — filled by GameStart*/, true /*isHost*/});
}

void MenuScreen::joinGame() {
    const std::string address = m_serverAddressBuffer.data();
    LOG_I("Joining game at {}:{}", address, config().serverPort.get());

    if (!m_network.connect(address, static_cast<uint16_t>(config().serverPort.get()))) {
        LOG_E("Failed to connect to {}:{}", address, config().serverPort.get());
        return;
    }

    LOG_I("Connected to server");
    queueRequest(screen::OpenLobbyRequest{0 /*localPlayerId — filled by GameStart*/, false /*isHost*/});
}

}  // namespace pacman::client::screens
