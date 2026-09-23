#pragma once
#include <SFML/Graphics.hpp>
#include "FrameAnimation.h"

// A texture sliced into a columns x rows grid of equal frames, played back
// via an owned FrameAnimation. Turns "which frame is current" into a
// texture rect and applies it to an sf::Sprite. Knows nothing about
// gameplay - just frame layout, timing, and rendering.
class SpriteSheet
{
public:
    SpriteSheet(const sf::Texture& texture, int columns, int rows, float frameDuration,
        float displaySize, bool looping = true);

    void update(float dt);
    void reset();
    bool isFinished() const;
    void applyCurrentFrame(sf::Sprite& sprite) const;
    float getBaseScale() const;

private:
    sf::IntRect getCurrentFrameRect() const;

    const sf::Texture* m_texture;
    int m_columns;
    int m_rows;
    sf::Vector2f m_frameOrigin; // center of one frame, not the whole sheet
    float m_baseScale;
    FrameAnimation m_animation;
};
