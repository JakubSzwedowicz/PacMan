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
  PacMan() : MovingEntity(EntityType::PAC_MAN) {}
  void update(std::chrono::milliseconds deltaTime, const Level::Board_t &board,
              const Level::Pacmans_t &pacmans,
              const Level::Ghosts_t &ghosts) override;

private:
  PacManState m_pacManState = PacManState::NORMAL;
};

} // namespace Entities
} // namespace GameObjects
} // namespace PacMan

#endif // PACMAN_H
