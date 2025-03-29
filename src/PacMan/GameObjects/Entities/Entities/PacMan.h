//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef PACMAN_H
#define PACMAN_H

#include "EntityType.h"
#include "MovingEntity.h"

#include <cstdint>


namespace PacMan {
namespace GameObjects {
namespace Entities {

enum class PacManState : uint8_t { NORMAL, EMPOWERED };

class PacMan : public MovingEntity {
public:
  PacMan(Level* level) : MovingEntity(EntityType::PAC_MAN, level) {}
  void update(std::chrono::time_point<std::chrono::steady_clock> timePoints) override;

private:
  PacManState m_pacManState = PacManState::NORMAL;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // PACMAN_H
