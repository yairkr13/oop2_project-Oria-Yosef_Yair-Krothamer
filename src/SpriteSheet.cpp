#include "SpriteSheet.h"
#include <algorithm>

namespace
{
    // One frame's real pixel size - not the whole sheet's - is what
    // origin/scale need to be based on (see the constructor below).
    sf::Vector2f computeFrameSize(const sf::Texture& texture, int columns, int rows)
    {
        sf::Vector2u sheetSize = texture.getSize();
        return {
            static_cast<float>(sheetSize.x) / static_cast<float>(std::max(columns, 1)),
            static_cast<float>(sheetSize.y) / static_cast<float>(std::max(rows, 1))
        };
    }
}

SpriteSheet::SpriteSheet(const sf::Texture& texture, int columns, int rows, float frameDuration,
    float displaySize, bool looping)
    : m_texture(&texture), m_columns(std::max(columns, 1)), m_rows(std::max(rows, 1)),
    m_animation(m_columns * m_rows, frameDuration, looping)
{
    sf::Vector2f frameSize = computeFrameSize(texture, columns, rows);
    m_frameOrigin = { frameSize.x / 2.f, frameSize.y / 2.f };

    float maxFrameDim = std::max(frameSize.x, frameSize.y);
    m_baseScale = (maxFrameDim > 0.f) ? (displaySize / maxFrameDim) : 1.f;
}

void SpriteSheet::update(float dt)
{
    m_animation.update(dt);
}

void SpriteSheet::reset()
{
    m_animation.reset();
}

bool SpriteSheet::isFinished() const
{
    return m_animation.isFinished();
}

sf::IntRect SpriteSheet::getCurrentFrameRect() const
{
    sf::Vector2u sheetSize = m_texture->getSize();
    int frameWidth = static_cast<int>(sheetSize.x) / m_columns;
    int frameHeight = static_cast<int>(sheetSize.y) / m_rows;

    int frameIndex = m_animation.getCurrentFrameIndex();
    int col = frameIndex % m_columns;
    int row = frameIndex / m_columns;

    return sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight });
}

void SpriteSheet::applyCurrentFrame(sf::Sprite& sprite) const
{
    // Same texture, different visible rect each call - only which pixels
    // of which texture the sprite shows changes here, so switching frames
    // can't make it jump around on its own.
    sprite.setTexture(*m_texture, false);
    sprite.setTextureRect(getCurrentFrameRect());
    sprite.setOrigin(m_frameOrigin);
}
