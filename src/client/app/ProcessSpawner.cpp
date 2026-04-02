#include "client/app/ProcessSpawner.hpp"

#include <Utils/Logging/LoggerMacros.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <string>

namespace pacman::client {

ProcessSpawner::ProcessSpawner(ProcessSpawner&& o) noexcept
    : m_pid(o.m_pid), m_notifyReadFd(o.m_notifyReadFd) {
    o.m_pid = -1;
    o.m_notifyReadFd = -1;
}

ProcessSpawner& ProcessSpawner::operator=(ProcessSpawner&& o) noexcept {
    if (this != &o) {
        kill();
        m_pid = o.m_pid;
        m_notifyReadFd = o.m_notifyReadFd;
        o.m_pid = -1;
        o.m_notifyReadFd = -1;
    }
    return *this;
}

bool ProcessSpawner::spawn(const std::string& executable, const std::vector<std::string>& args) {
    if (m_pid > 0 && isRunning()) {
        LOG_W("spawn() called while child {} is still running — killing it first", m_pid);
        kill();
    }
    if (m_notifyReadFd >= 0) {
        ::close(m_notifyReadFd);
        m_notifyReadFd = -1;
    }

    int pipeFds[2];
    if (::pipe(pipeFds) < 0) {
        LOG_E("pipe() failed: {}", std::strerror(errno));
        return false;
    }

    m_pid = ::fork();
    if (m_pid < 0) {
        LOG_E("fork() failed: {}", std::strerror(errno));
        ::close(pipeFds[0]);
        ::close(pipeFds[1]);
        m_pid = -1;
        return false;
    }

    if (m_pid == 0) {
        // Child: close the read end — we only write notifications.
        // The write end (pipeFds[1]) is left open and inherited across exec.
        ::close(pipeFds[0]);

        // Tell the server which fd to write its bound port to.
        const std::string notifyFdStr = std::to_string(pipeFds[1]);
        std::vector<const char*> argv;
        argv.reserve(args.size() + 4);
        argv.push_back(executable.c_str());
        for (const auto& a : args) argv.push_back(a.c_str());
        argv.push_back("--notifyFd");
        argv.push_back(notifyFdStr.c_str());
        argv.push_back(nullptr);

        ::execvp(executable.c_str(), const_cast<char* const*>(argv.data()));
        ::_exit(127);  // exec failed
    }

    // Parent: close the write end, keep the read end.
    ::close(pipeFds[1]);
    m_notifyReadFd = pipeFds[0];

    LOG_I("Spawned '{}' as pid {}", executable, m_pid);
    return true;
}

std::optional<std::string> ProcessSpawner::readLine(int timeoutMs) {
    if (m_notifyReadFd < 0) return std::nullopt;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_notifyReadFd, &fds);
    timeval tv{timeoutMs / 1000, (timeoutMs % 1000) * 1000};

    if (::select(m_notifyReadFd + 1, &fds, nullptr, nullptr, &tv) <= 0)
        return std::nullopt;

    std::string line;
    char c;
    while (::read(m_notifyReadFd, &c, 1) == 1) {
        if (c == '\n') break;
        line += c;
    }
    return line.empty() ? std::nullopt : std::make_optional(std::move(line));
}

void ProcessSpawner::kill() {
    if (m_notifyReadFd >= 0) {
        ::close(m_notifyReadFd);
        m_notifyReadFd = -1;
    }
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
