#pragma once

namespace pacman::server::phases {

class Phase {
public:
  virtual ~Phase() = default;

  virtual void onEnter() = 0;
  virtual void onExit() = 0;
  virtual void update(float dt) = 0;
};

} // namespace pacman::server::phases
