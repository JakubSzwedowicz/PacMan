#pragma once

#include <Utils/Logging/Logger.h>

#include <string>
#include <vector>

namespace pacman::client {

// Spawns and manages a single child process (e.g., PacManServer).
// Uses fork()+execvp() on Linux.
class ProcessSpawner {
   public:
    ProcessSpawner() = default;
    ~ProcessSpawner() { kill(); }

    ProcessSpawner(const ProcessSpawner&) = delete;
    ProcessSpawner& operator=(const ProcessSpawner&) = delete;
    ProcessSpawner(ProcessSpawner&&) noexcept;
    ProcessSpawner& operator=(ProcessSpawner&&) noexcept;

    // Forks and exec's the given executable with args.
    // Returns false if fork/execvp fails.
    [[nodiscard]] bool spawn(const std::string& executable, const std::vector<std::string>& args);

    // Sends SIGTERM to the child and waits for it to exit.
    void kill();

    // Returns true if the child is still running (non-blocking check).
    [[nodiscard]] bool isRunning() const;

    // Returns the PID of the spawned process (-1 if not spawned).
    [[nodiscard]] int pid() const { return m_pid; }

   private:
    int m_pid = -1;
    Utils::Logging::Logger m_logger{"ProcessSpawner"};
};

}  // namespace pacman::client
