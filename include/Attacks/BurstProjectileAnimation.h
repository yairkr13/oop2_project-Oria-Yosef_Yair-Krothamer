#pragma once
#include "Attacks/StaggeredInstancesAnimation.h"

// A burst of staggered projectiles flying straight from attacker to target,
// overlapping in flight. Impact fires once, on the last projectile's
// arrival - one burst still counts as a single attack. Used by Blue,
// Henrietta, and Barzilla's empowered attack.
class BurstProjectileAnimation : public StaggeredInstancesAnimation
{
public:
    BurstProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
        int projectileCount, float launchInterval, float travelDuration, float size);

private:
    void positionInstance(Instance& instance, float progress) override;

    sf::Vector2f m_origin;
    sf::Vector2f m_target;
};
