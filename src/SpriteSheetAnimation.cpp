#include "SpriteSheetAnimation.h"
#include <algorithm>

SpriteSheetAnimation::SpriteSheetAnimation(int columns, int rows, float frameDuration, bool looping)
    : m_columns(std::max(columns, 1)), m_rows(std::max(rows, 1)),
    m_frameCount(std::max(columns, 1) * std::max(rows, 1)), m_frameDuration(frameDuration),
    m_looping(looping)
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

bool SpriteSheetAnimation::isFinished() const
{
    if (m_looping || m_frameDuration <= 0.f || m_frameCount <= 0) return false;
    return m_elapsed >= m_frameDuration * static_cast<float>(m_frameCount);
}

sf::IntRect SpriteSheetAnimation::getCurrentFrameRect(sf::Vector2u textureSize) const
{
    int frameWidth = static_cast<int>(textureSize.x) / m_columns;
    int frameHeight = static_cast<int>(textureSize.y) / m_rows;

    int frameIndex = 0;
    if (m_frameDuration > 0.f && m_frameCount > 0)
    {
        int rawIndex = static_cast<int>(m_elapsed / m_frameDuration);
        // Looping wraps forever (unchanged from before this parameter
        // existed); non-looping clamps at the last frame instead of
        // wrapping back to the first once done, so it visibly holds there
        // rather than restarting - isFinished() above is what reports that
        // this happened.
        frameIndex = m_looping ? (rawIndex % m_frameCount)
                                : std::min(rawIndex, m_frameCount - 1);
    }

    int col = frameIndex % m_columns;
    int row = frameIndex / m_columns;

    return sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight });
}
