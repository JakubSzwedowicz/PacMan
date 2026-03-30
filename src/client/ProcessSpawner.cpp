#include "client/ProcessSpawner.hpp"

#include <Utils/Logging/LoggerMacros.h>

#include <csignal>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace pacman::client {

ProcessSpawner::ProcessSpawner(ProcessSpawner &&o) noexcept : m_pid(o.m_pid) { o.m_pid = -1; }

ProcessSpawner &ProcessSpawner::operator=(ProcessSpawner &&o) noexcept {
    if (this != &o) {
        m_pid = o.m_pid;
        o.m_pid = -1;
    }
    return *this;
}

bool ProcessSpawner::spawn(const std::string &executable, const std::vector<std::string> &args) {
    if (m_pid > 0 && isRunning()) {
        LOG_W("spawn() called while child {} is still running", m_pid);
    }

    m_pid = ::fork();
    if (m_pid < 0) {
        LOG_E("fork() failed: {}", std::strerror(errno));
        m_pid = -1;
        return false;
    }

    if (m_pid == 0) {
        // Child process — build argv and exec
        std::vector<const char *> argv;
        argv.reserve(args.size() + 2);
        argv.push_back(executable.c_str());
        for (const auto &a : args) argv.push_back(a.c_str());
        argv.push_back(nullptr);
        ::execvp(executable.c_str(), const_cast<char *const *>(argv.data()));
        ::_exit(127);  // exec failed
    }

    // Parent
    LOG_I("Spawned '{}' as pid {}", executable, m_pid);
    return true;
}

void ProcessSpawner::kill() {
    if (m_pid <= 0) return;
    LOG_I("Sending SIGTERM to pid {}", m_pid);
    ::kill(m_pid, SIGTERM);
    ::waitpid(m_pid, nullptr, 0);
    m_pid = -1;
}

bool ProcessSpawner::isRunning() const {
    if (m_pid <= 0) return false;
    return ::waitpid(m_pid, nullptr, WNOHANG) == 0;
}

}  // namespace pacman::client
