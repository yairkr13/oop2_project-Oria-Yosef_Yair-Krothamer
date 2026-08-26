#pragma once
#include <SFML/Graphics.hpp>
#include "FrameAnimation.h"

// A texture sliced into a uniform `columns` x `rows` grid of equal-sized
// frames, played back over time via an owned FrameAnimation. Knows how to
// turn "which frame is current" into an actual texture rect, and how to
// apply that frame (texture + rect + origin) onto an sf::Sprite - the
// generic, reusable half of what a caller's sprite-sheet-driven animation
// state needs (frame layout, timing, and rendering), independent of
// *when* that state is active or *what* it represents. Knows nothing
// about monsters, movement, attacking, or any other gameplay concept - a
// caller (Monster, or any other animated entity later) decides which of
// its own sheets is active right now and asks that one to update/apply
// itself; this class only knows how to play itself once told to.
class SpriteSheet
{
public:
    // `displaySize` is the desired on-screen max dimension (in pixels) of
    // one frame, independent of the sheet's native resolution - the same
    // "size, not native pixels" approach already used elsewhere in this
    // project (e.g. SpinningProjectileAnimation's `size` parameter).
    // `looping` mirrors FrameAnimation's: true (default) for a continuous
    // animation, false for a one-shot that holds on its last frame.
    SpriteSheet(const sf::Texture& texture, int columns, int rows, float frameDuration,
        float displaySize, bool looping = true);

    void update(float dt);
    void reset();
    bool isFinished() const;

    // Applies this sheet's current frame to `sprite`: texture, texture
    // rect (the current frame's slice of the sheet) and origin (the
    // center of ONE frame, not the whole sheet, so positioning/rotation
    // behaves the same as it would for a plain static sprite). Deliberately
    // leaves position and scale untouched - see getBaseScale() below,
    // since those depend on things this class knows nothing about (e.g. a
    // Monster's own on-board position, or its left/right mirroring).
    void applyCurrentFrame(sf::Sprite& sprite) const;

    // This sheet's own base scale (derived from one frame's real pixel
    // size against `displaySize`, not the sheet's native resolution) - a
    // caller combines this with whatever else it needs (e.g. Monster's
    // side-based horizontal flip) to arrive at the sprite's final scale.
    float getBaseScale() const { return m_baseScale; }

private:
    sf::IntRect getCurrentFrameRect() const;

    const sf::Texture* m_texture;
    int m_columns;
    int m_rows;
    sf::Vector2f m_frameOrigin;
    float m_baseScale;
    FrameAnimation m_animation;
};
