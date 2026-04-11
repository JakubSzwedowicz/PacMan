#include "client/ScreenManagement/screens/MenuScreen.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <filesystem>
#include <string>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#endif

namespace pacman::client::screens {

static std::string serverBinaryPath() {
#if defined(__linux__)
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len < 0) return "PacManServer";
    buf[len] = '\0';
    return (std::filesystem::path(buf).parent_path().parent_path() / "server" / "PacManServer").string();
#elif defined(__APPLE__)
    char buf[PATH_MAX];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) != 0) return "PacManServer";
    return (std::filesystem::canonical(buf).parent_path().parent_path() / "server" / "PacManServer").string();
#else
    return "PacManServer";
#endif
}


MenuScreen::MenuScreen(network::ClientNetwork& network, ProcessSpawner& spawner)
    : m_network(network), m_spawner(spawner) {}

void MenuScreen::onEnter() {
    m_serverAddress = config().serverAddress.get();
    m_serverPort = std::to_string(config().serverPort.get());
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

    ImGui::SetNextWindowPos(
        ImVec2((static_cast<float>(size.x) - panelWidth) * 0.5f, (static_cast<float>(size.y) - panelHeight) * 0.5f),
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
    ImGui::InputText("Server IP", &m_serverAddress);
    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    ImGui::InputText("Port", &m_serverPort, ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Quit", ImVec2(buttonWidth, 40))) queueRequest(screen::QuitAppRequest{});

    ImGui::End();
}

void MenuScreen::hostGame() {
    auto serverBin = serverBinaryPath();
    // Port 0 = OS assigns an ephemeral port. The server prints "PORT=<n>" to stdout.
    std::vector<std::string> args{"--port", "0", "--mapPath", config().mapPath.get()};

    if (!m_spawner.spawn(serverBin, args)) {
        LOG_E("Failed to spawn server at '{}'", serverBin);
        return;
    }

    // Block until the server prints its bound port (or 2 s timeout).
    const auto line = m_spawner.readLine(2000);
    if (!line || line->rfind("PORT=", 0) != 0) {
        LOG_E("Did not receive port from server (got: {})", line.value_or("<nothing>"));
        m_spawner.kill();
        return;
    }
    const auto port = static_cast<uint16_t>(std::stoi(line->substr(5)));
    LOG_I("Server bound to port {}", port);

    if (!m_network.connect("127.0.0.1", port)) {
        LOG_E("Failed to connect to local server on port {}", port);
        m_spawner.kill();
        return;
    }

    LOG_I("Connected to local server on port {}", port);
    queueRequest(screen::OpenLobbyRequest{0 /*localPlayerId — filled by GameStart*/, true /*isHost*/});
}

void MenuScreen::joinGame() {
    const auto port = static_cast<uint16_t>(std::stoi(m_serverPort));
    LOG_I("Joining game at {}:{}", m_serverAddress, port);

    if (!m_network.connect(m_serverAddress, port)) {
        LOG_E("Failed to connect to {}:{}", m_serverAddress, port);
        return;
    }

    LOG_I("Connected to server");
    queueRequest(screen::OpenLobbyRequest{0 /*localPlayerId — filled by GameStart*/, false /*isHost*/});
}

}  // namespace pacman::client::screens
