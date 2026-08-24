#pragma once
#include "Attacks/AttackAnimation.h"
#include <vector>

// Used by Muffintop for its special ability (the HealEffect that plays on
// the healed ally).

// A "rising effect" animation: one or more copies of a sprite, arranged
// horizontally around a fixed board position, that rise straight up over
// their duration, then trigger the impact callback once every copy has
// reached the top of its rise.
//
// Structurally different from the three normal-attack animations
// (SplashAttackAnimation, SpinningProjectileAnimation, BurstProjectileAnimation):
// none of those fit here, because this effect does not travel from an
// attacker to a target - it plays entirely in place, at a single entity's
// own position, which is exactly the shape a Special Ability's effect on a
// single target (rather than a projectile between two) tends to need.
// Where it does borrow from a sibling: multiple simultaneous, individually
// staggered copies converging on one shared completion is the same idea
// BurstProjectileAnimation already uses for its projectiles - reused here
// instead of inventing a second "many instances of one effect" mechanism.
//
// Deliberately knows nothing about Muffintop, healing, HP, or which entity
// it's attached to - it only animates the rising sprite(s) and reports when
// they're all done, exactly like its AttackAnimation siblings. Generic and
// reusable by any future Special wanting this "rises in place" look, with a
// different texture/duration/size/rise distance/instance count.
class RisingEffectAnimation : public AttackAnimation
{
public:
    // `position` is the starting (bottom) point the effect rises from - the
    // center that instances are arranged around when instanceCount > 1.
    // `riseDistance` is how far up (in pixels) each instance travels before
    // finishing - callers typically derive this from the board's own tile
    // size (e.g. Config::TILE_RADIUS), not from the texture. `size`: desired
    // on-screen max dimension per instance, independent of texture
    // resolution - same convention as the other AttackAnimation subclasses.
    // `instanceCount`: how many copies rise together (1 - the default -
    // reproduces the original single-sprite behavior exactly). `horizontalSpacing`:
    // pixel distance between adjacent instances, centered on `position`
    // (only meaningful when instanceCount > 1). `staggerDelay`: seconds of
    // start-delay between each successive instance - a subtle offset so
    // several instances read as one bloom rather than perfectly rigid twins;
    // 0 (the default) starts them all together.
    RisingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
        float riseDistance, float duration, float size,
        int instanceCount = 1, float horizontalSpacing = 0.f, float staggerDelay = 0.f);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

private:
    struct Riser
    {
        sf::Sprite sprite;
        sf::Vector2f bottomPosition;
        float startDelay;
        float elapsed = 0.f;
        bool started = false;
        bool finished = false;
    };

    float m_riseDistance;
    float m_duration;
    float m_totalElapsed = 0.f;
    int m_finishedCount = 0;
    std::vector<Riser> m_risers;
};
