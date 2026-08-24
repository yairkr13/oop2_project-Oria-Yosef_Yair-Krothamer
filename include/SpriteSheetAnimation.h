#pragma once
#include <SFML/Graphics.hpp>

// A small, generic looping frame animator over a uniform-grid sprite sheet:
// given a texture already sliced into `columns` x `rows` equal-sized
// frames, advances through them (row by row, left to right) at a fixed
// `frameDuration`, looping back to the first frame after the last.
//
// Deliberately minimal and reusable - knows nothing about monsters,
// movement, textures, or gameplay; it only tracks "which frame is current"
// over time and reports that frame's texture-rect. Frame width/height are
// derived here from the actual texture size passed to
// getCurrentFrameRect(), never hardcoded, so this stays correct regardless
// of the sheet's real resolution as long as the grid is uniform.
//
// Not an AttackAnimation: that hierarchy is for one-shot attack/special
// effects with impact/finish semantics (see Attacks/AttackAnimation.h).
// This is a continuous, looping, state-driven animation with no impact or
// finish concept at all - a different enough shape that reusing that base
// would only force an ill-fitting abstraction.
class SpriteSheetAnimation
{
public:
    SpriteSheetAnimation(int columns, int rows, float frameDuration);

    // Advances the animation clock - call only while the animation should
    // actually be playing (e.g. only while the owning monster isMoving()).
    void update(float dt);

    // Restarts from the first frame - call whenever playback should begin
    // fresh (e.g. movement has stopped, ready for the next walk).
    void reset();

    // The current frame's texture-rect, computed from the real texture
    // size (not a stored/assumed one) divided by columns/rows.
    sf::IntRect getCurrentFrameRect(sf::Vector2u textureSize) const;

private:
    int m_columns;
    int m_rows;
    int m_frameCount;
    float m_frameDuration;
    float m_elapsed = 0.f;
};
