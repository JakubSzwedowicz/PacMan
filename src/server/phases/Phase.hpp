#pragma once

#include "server/phases/PhaseRequest.hpp"

namespace pacman::server::phases {

class Phase {
public:
    virtual ~Phase() = default;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;

    // Returns PhaseRunning while the phase is active.
    // Returns a transition request when it wants GameRunner to advance the FSM.
    virtual PhaseRequest update(float dt) = 0;
};

} // namespace pacman::server::phases
