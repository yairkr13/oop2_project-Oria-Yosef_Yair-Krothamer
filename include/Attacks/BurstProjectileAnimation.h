#pragma once
#include "Attacks/StaggeredInstancesAnimation.h"

// Used by Blue (normal attack, WindBlast), Henrietta (normal attack,
// FlameWeb), and Barzilla (special ability, the grouped FireBlast
// projectile for his Empowered Attack).

// A "burst" style attack animation: several identical copies of one texture,
// launched one after another (staggered) from the attacker toward the
// target, each then flying independently in a straight line. Visually
// distinct from the other two AttackAnimation subclasses - unlike
// SplashAttackAnimation (a single band that reveals in place at the
// attacker) or SpinningProjectileAnimation (a single spinning projectile),
// here there are several simultaneous, independently-timed projectiles that
// are expected to overlap in flight.
//
// Impact fires exactly once, the instant the LAST projectile reaches the
// target - every earlier arrival is purely visual, so a burst of 5 shots
// still represents a single normal attack: one action consumed, one
// instance of damage applied.
//
// Deliberately knows nothing about which monster is using it, which texture
// it was given, damage values, Tile, or combat rules - it only animates a
// staggered burst and reports when the burst has finished landing, exactly
// like its sibling AttackAnimation subclasses. Generic and reusable by any
// monster wanting this "rapid burst" look with a different
// texture/count/timing/size.
class BurstProjectileAnimation : public StaggeredInstancesAnimation
{
public:
    // `projectileCount`: how many copies fly. `launchInterval`: seconds
    // between each successive launch (the stagger - shots overlap in
    // flight by design). `travelDuration`: seconds each individual
    // projectile takes to fly from origin to target once launched. `size`:
    // desired on-screen max dimension per projectile, independent of
    // texture resolution and travel distance - same convention as
    // SplashAttackAnimation's `thickness`/SpinningProjectileAnimation's `size`.
    BurstProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
        int projectileCount, float launchInterval, float travelDuration, float size);

private:
    // The one thing StaggeredInstancesAnimation can't share - every
    // projectile flies the same straight line from m_origin to m_target,
    // so this is a plain lerp by progress.
    void positionInstance(Instance& instance, float progress) override;

    sf::Vector2f m_origin;
    sf::Vector2f m_target;
};
