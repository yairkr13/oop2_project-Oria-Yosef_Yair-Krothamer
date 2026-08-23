#include "Attacks/RisingEffectAnimation.h"
#include <algorithm>

RisingEffectAnimation::RisingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
    float riseDistance, float duration, float size)
    : m_sprite(texture), m_bottomPosition(position), m_riseDistance(riseDistance), m_duration(duration)
{
    sf::Vector2u textureSize = texture.getSize();
    float maxTextureDim = std::max(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
    float scale = (maxTextureDim > 0.f) ? (size / maxTextureDim) : 1.f;

    m_sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });
    m_sprite.setScale({ scale, scale });
    m_sprite.setPosition(m_bottomPosition);
}

void RisingEffectAnimation::update(float dt)
{
    if (isFinished()) return;

    m_elapsed += dt;
    float progress = (m_duration > 0.f) ? std::clamp(m_elapsed / m_duration, 0.f, 1.f) : 1.f;

    // Straight vertical rise - screen-space "up" is simply a smaller y, no
    // direction/rotation math needed since this never travels sideways.
    m_sprite.setPosition({ m_bottomPosition.x, m_bottomPosition.y - m_riseDistance * progress });

    if (progress >= 1.f)
    {
        fireImpact(); // the effect (e.g. the actual heal) applies exactly when the rise completes
        finish();
    }
}

void RisingEffectAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
