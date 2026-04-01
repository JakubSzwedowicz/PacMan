#pragma once

#include "core/Common.hpp"
#include "core/ecs/Components.hpp"

namespace pacman::client::input {

struct InputSnapshot {
    core::ecs::Direction direction = core::ecs::Direction::None;
    core::Tick tick = 0;
    bool escapePressed = false;
};

}  // namespace pacman::client::input
