#pragma once
#include "Attacks/AttackAnimation.h"

// A "rising effect" animation: a sprite anchored at one fixed board
// position that rises straight up over its duration, then triggers the
// impact callback once it reaches the top of its rise.
//
// Structurally different from the three normal-attack animations
// (SplashAttackAnimation, SpinningProjectileAnimation, BurstProjectileAnimation):
// none of those fit here, because this effect does not travel from an
// attacker to a target - it plays entirely in place, at a single entity's
// own position, which is exactly the shape a Special Ability's effect on a
// single target (rather than a projectile between two) tends to need.
//
// Deliberately knows nothing about Muffintop, healing, HP, or which entity
// it's attached to - it only animates a rising sprite and reports when it's
// done, exactly like its AttackAnimation siblings. Generic and reusable by
// any future Special wanting this "rises in place" look, with a different
// texture/duration/size/rise distance.
class RisingEffectAnimation : public AttackAnimation
{
public:
    // `position` is the starting (bottom) point the effect rises from.
    // `riseDistance` is how far up (in pixels) it travels before finishing -
    // callers typically derive this from the board's own tile size (e.g.
    // Config::TILE_RADIUS), not from the texture. `size`: desired on-screen
    // max dimension, independent of texture resolution - same convention as
    // the other AttackAnimation subclasses.
    RisingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
        float riseDistance, float duration, float size);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_bottomPosition;
    float m_riseDistance;
    float m_duration;
    float m_elapsed = 0.f;
};
