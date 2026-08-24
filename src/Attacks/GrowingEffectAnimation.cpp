#include "Attacks/GrowingEffectAnimation.h"
#include <algorithm>
#include <cstdint>

GrowingEffectAnimation::GrowingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
    float size, float growDuration, float holdDuration, float fadeDuration)
    : m_sprite(texture), m_growDuration(growDuration), m_holdDuration(holdDuration), m_fadeDuration(fadeDuration)
{
    sf::Vector2u textureSize = texture.getSize();
    float maxTextureDim = std::max(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
    m_fullScale = (maxTextureDim > 0.f) ? (size / maxTextureDim) : 1.f;

    // Centered origin so growth expands symmetrically around `position` -
    // never from a corner/edge.
    m_sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });
    m_sprite.setPosition(position);
    m_sprite.setScale({ 0.f, 0.f }); // starts at zero size - grows from nothing
}

void GrowingEffectAnimation::update(float dt)
{
    if (isFinished()) return;

    m_elapsed += dt;

    switch (m_phase)
    {
    case Phase::Growing:
    {
        float progress = (m_growDuration > 0.f) ? std::clamp(m_elapsed / m_growDuration, 0.f, 1.f) : 1.f;
        float scale = m_fullScale * progress;
        m_sprite.setScale({ scale, scale });

        if (progress >= 1.f)
        {
            fireImpact(); // the barrier is "up" exactly when it reaches full size
            m_phase = Phase::Holding;
            m_elapsed = 0.f;
        }
        break;
    }
    case Phase::Holding:
        if (m_elapsed >= m_holdDuration)
        {
            if (m_fadeDuration > 0.f)
            {
                m_phase = Phase::FadingOut;
                m_elapsed = 0.f;
            }
            else
            {
                finish();
            }
        }
        break;
    case Phase::FadingOut:
    {
        float progress = std::clamp(m_elapsed / m_fadeDuration, 0.f, 1.f);
        sf::Color color = m_sprite.getColor();
        color.a = static_cast<std::uint8_t>(255.f * (1.f - progress));
        m_sprite.setColor(color);

        if (progress >= 1.f)
            finish();
        break;
    }
    }
}

void GrowingEffectAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
