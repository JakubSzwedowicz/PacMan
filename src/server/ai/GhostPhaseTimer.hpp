#pragma once

namespace pacman::server::ai {

// Tracks the current global ghost phase (Scatter / Chase / Frightened) and
// advances timers each tick. AISystem queries this to know which behaviour to
// apply to every ghost.
//
// Phase sequence:  Scatter 7 s → Chase 20 s → Scatter 7 s → Chase ∞
// Power-pellet:    interrupts to Frightened 10 s, then resumes prior phase.
class GhostPhaseTimer {
   public:
    GhostPhaseTimer() = default;

    void update(float dt);
    void onPowerPelletEaten();

    [[nodiscard]] bool isScatter() const { return !m_frightened && m_inScatter; }
    [[nodiscard]] bool isFrightened() const { return m_frightened; }

   private:
    float m_phaseTimer = 7.0f;  // seconds until next scatter/chase flip
    bool m_inScatter = true;
    float m_frightenedTimer = 0.0f;
    bool m_frightened = false;
};

}  // namespace pacman::server::ai
