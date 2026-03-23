#include "server/ai/GhostPhaseTimer.hpp"

namespace pacman::server::ai {

void GhostPhaseTimer::update(float dt) {
  if (m_frightened) {
    m_frightenedTimer -= dt;
    if (m_frightenedTimer <= 0.0f) {
      m_frightened = false;
      m_phaseTimer = m_inScatter ? 7.0f : 20.0f;
    }
    return;
  }

  m_phaseTimer -= dt;
  if (m_phaseTimer <= 0.0f) {
    m_inScatter = !m_inScatter;
    m_phaseTimer = m_inScatter ? 7.0f : 20.0f;
  }
}

void GhostPhaseTimer::onPowerPelletEaten() {
  m_frightened = true;
  m_frightenedTimer = 10.0f;
}

} // namespace pacman::server::ai
