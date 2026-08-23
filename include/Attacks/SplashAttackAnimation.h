#pragma once
#include "Attacks/AttackAnimation.h"

// A "splash" style attack animation: a sprite anchored at the attacker,
// rotated to face the target, that visually grows/reveals from the
// attacker toward the target over its duration, then triggers the impact
// callback exactly when the reveal reaches the target.
//
// Works for any attacker->target direction (not just left-to-right): the
// reveal is a texture-rect clip along the sprite's own local X axis, and
// rotating the whole sprite to face the target carries that local axis to
// point the right way on screen regardless of which of the six board
// directions the attack travels in.
//
// Reusable as-is by any monster that wants this same "grows toward target"
// look with a different texture/duration/thickness - only a monster that
// needs a genuinely different visual mechanism (an arc, a melee swipe, an
// AoE burst, ...) needs a new AttackAnimation subclass.
class SplashAttackAnimation : public AttackAnimation
{
public:
    // `thickness` is the desired on-screen height (in pixels) of the
    // revealed band, independent of `origin`/`target`: it controls how
    // visually "thick" the splash looks, never how far it reaches. Callers
    // should derive it from the project's existing on-board sizing (e.g.
    // Config::MONSTER_BOARD_SIZE) rather than passing the texture's native
    // pixel size.
    SplashAttackAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
        float duration, float thickness);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

private:
    sf::Sprite m_sprite;
    sf::Vector2u m_textureSize;
    float m_duration;
    float m_elapsed = 0.f;
};
