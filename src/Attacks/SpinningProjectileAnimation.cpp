#include "Attacks/SpinningProjectileAnimation.h"
#include <algorithm>

SpinningProjectileAnimation::SpinningProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
    float duration, float size, float spinSpeedDegreesPerSecond)
    : m_sprite(texture), m_origin(origin), m_target(target), m_duration(duration),
    m_spinSpeedDegreesPerSecond(spinSpeedDegreesPerSecond)
{
    // Centered origin (unlike SplashAttackAnimation's edge-anchored origin):
    // this sprite travels as a whole rigid body, so it should spin about its
    // own center rather than about one edge.
    sf::Vector2u textureSize = texture.getSize();
    m_sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });

    // Uniform scale (preserves aspect ratio) sized against `size`, the same
    // idea Monster::Monster already uses for its own sprite (scale relative
    // to Config::MONSTER_BOARD_SIZE) - independent of travel distance,
    // which is handled entirely by position interpolation in update().
    float maxTextureDim = std::max(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
    float scale = (maxTextureDim > 0.f) ? (size / maxTextureDim) : 1.f;
    m_sprite.setScale({ scale, scale });

    m_sprite.setPosition(origin);
}

void SpinningProjectileAnimation::update(float dt)
{
    if (isFinished()) return;

    m_elapsed += dt;
    float progress = (m_duration > 0.f) ? std::clamp(m_elapsed / m_duration, 0.f, 1.f) : 1.f;

    // Straight-line travel from origin to target - correct in any
    // direction by construction, since it's a plain vector lerp rather than
    // anything assuming a fixed screen-space axis.
    sf::Vector2f pos = m_origin + (m_target - m_origin) * progress;
    m_sprite.setPosition(pos);

    // Continuous spin, entirely decoupled from travel progress/direction -
    // rotate() accumulates on top of whatever rotation the sprite already
    // has, so this alone produces a steady spin for the whole flight.
    m_sprite.rotate(sf::degrees(m_spinSpeedDegreesPerSecond * dt));

    if (progress >= 1.f)
    {
        fireImpact(); // damage applies exactly when the projectile reaches the target
        finish();     // Board discards us (via the owning Monster) next frame
    }
}

void SpinningProjectileAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
