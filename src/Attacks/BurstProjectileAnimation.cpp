#include "Attacks/BurstProjectileAnimation.h"
#include <cmath>
#include <algorithm>

BurstProjectileAnimation::BurstProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
    int projectileCount, float launchInterval, float travelDuration, float size)
    : m_origin(origin), m_target(target), m_travelDuration(travelDuration)
{
    sf::Vector2u textureSize = texture.getSize();
    float maxTextureDim = std::max(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
    float scale = (maxTextureDim > 0.f) ? (size / maxTextureDim) : 1.f;

    // All projectiles travel the same origin->target line, so they all
    // share one facing direction, computed once - rotating each sprite to
    // face the direction it flies makes the burst look directional
    // regardless of which of the six board directions the attack is in.
    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    sf::Angle facing = sf::radians(std::atan2(dy, dx));

    m_projectiles.reserve(static_cast<std::size_t>(std::max(projectileCount, 0)));
    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile p{ sf::Sprite(texture) };
        p.sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });
        p.sprite.setScale({ scale, scale });
        p.sprite.setRotation(facing);
        p.sprite.setPosition(origin);
        p.launchDelay = launchInterval * static_cast<float>(i); // staggered launch, not staggered arrival
        m_projectiles.push_back(std::move(p));
    }
}

void BurstProjectileAnimation::update(float dt)
{
    if (isFinished()) return;

    m_totalElapsed += dt;

    for (auto& p : m_projectiles)
    {
        if (p.arrived) continue;

        if (!p.launched)
        {
            if (m_totalElapsed < p.launchDelay) continue; // not this one's turn yet
            p.launched = true;
        }

        p.elapsed += dt;
        float progress = (m_travelDuration > 0.f) ? std::clamp(p.elapsed / m_travelDuration, 0.f, 1.f) : 1.f;
        p.sprite.setPosition(m_origin + (m_target - m_origin) * progress);

        if (progress >= 1.f)
        {
            p.arrived = true;
            ++m_arrivedCount;
        }
    }

    // Every projectile has landed - fire the shared impact callback exactly
    // once, regardless of how many shots were visually launched.
    if (m_arrivedCount >= static_cast<int>(m_projectiles.size()))
    {
        fireImpact();
        finish();
    }
}

void BurstProjectileAnimation::draw(sf::RenderWindow& window) const
{
    for (auto const& p : m_projectiles)
    {
        if (p.launched)
            window.draw(p.sprite);
    }
}
