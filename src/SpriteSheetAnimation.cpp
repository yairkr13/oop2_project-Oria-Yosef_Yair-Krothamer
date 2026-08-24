#include "SpriteSheetAnimation.h"
#include <algorithm>

SpriteSheetAnimation::SpriteSheetAnimation(int columns, int rows, float frameDuration)
    : m_columns(std::max(columns, 1)), m_rows(std::max(rows, 1)),
    m_frameCount(std::max(columns, 1) * std::max(rows, 1)), m_frameDuration(frameDuration)
{
}

void SpriteSheetAnimation::update(float dt)
{
    m_elapsed += dt;
}

void SpriteSheetAnimation::reset()
{
    m_elapsed = 0.f;
}

sf::IntRect SpriteSheetAnimation::getCurrentFrameRect(sf::Vector2u textureSize) const
{
    int frameWidth = static_cast<int>(textureSize.x) / m_columns;
    int frameHeight = static_cast<int>(textureSize.y) / m_rows;

    int frameIndex = 0;
    if (m_frameDuration > 0.f && m_frameCount > 0)
        frameIndex = static_cast<int>(m_elapsed / m_frameDuration) % m_frameCount;

    int col = frameIndex % m_columns;
    int row = frameIndex / m_columns;

    return sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight });
}
