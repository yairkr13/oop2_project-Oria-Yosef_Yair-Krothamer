#include "Attacks/SpinningProjectileAnimation.h"
#include "SpriteUtils.h"
#include <algorithm>

SpinningProjectileAnimation::SpinningProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
    float duration, float size, float spinSpeedDegreesPerSecond)
    : m_sprite(texture), m_origin(origin), m_target(target), m_duration(duration),
    m_spinSpeedDegreesPerSecond(spinSpeedDegreesPerSecond)
{
    // Centered origin - the sprite travels as a rigid body, so it spins
    // about its own center rather than an edge.
    sf::Vector2u textureSize = texture.getSize();
    m_sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });

    float scale = SpriteUtils::maxDimensionScale(textureSize, size);
    m_sprite.setScale({ scale, scale });

    m_sprite.setPosition(origin);
}

void SpinningProjectileAnimation::update(float dt)
{
    if (isFinished()) return;

    m_elapsed += dt;
    float progress = (m_duration > 0.f) ? std::clamp(m_elapsed / m_duration, 0.f, 1.f) : 1.f;

    // Straight-line travel, decoupled from the spin below.
    sf::Vector2f pos = m_origin + (m_target - m_origin) * progress;
    m_sprite.setPosition(pos);

    m_sprite.rotate(sf::degrees(m_spinSpeedDegreesPerSecond * dt));

    if (progress >= 1.f)
    {
        fireImpact(); // damage applies exactly on arrival
        finish();
    }
}

void SpinningProjectileAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
