#include "Attacks/BurstProjectileAnimation.h"
#include <cmath>

BurstProjectileAnimation::BurstProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
    int projectileCount, float launchInterval, float travelDuration, float size)
    : m_origin(origin), m_target(target)
{
    initInstances(texture, projectileCount, launchInterval, size, travelDuration,
        [origin](int) { return origin; }); // every projectile launches from the same shared origin

    // All projectiles travel the same origin->target line, so they all
    // share one facing direction, computed once - rotating each sprite to
    // face the direction it flies makes the burst look directional
    // regardless of which of the six board directions the attack is in.
    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    sf::Angle facing = sf::radians(std::atan2(dy, dx));
    for (auto& inst : m_instances)
        inst.sprite.setRotation(facing);
}

void BurstProjectileAnimation::positionInstance(Instance& instance, float progress)
{
    instance.sprite.setPosition(m_origin + (m_target - m_origin) * progress);
}
