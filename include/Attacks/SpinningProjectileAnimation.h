#pragma once
#include "Attacks/AttackAnimation.h"

// Used by Muffintop (normal attack, muffin shot) and Blue (special ability,
// the WindEffect that travels through the target during Knockback).

// A projectile-style attack animation: a sprite that flies in a straight
// line from the attacker to the target, spinning continuously while it
// travels, then triggers the impact callback the instant it arrives.
//
// Unlike SplashAttackAnimation (which stays anchored at the attacker and
// reveals toward the target), this sprite itself moves: position is a
// straight-line interpolation from origin to target, entirely independent
// of the spin. That independence is what makes it correct in every attack
// direction regardless of how many times it has spun by the time it lands -
// the two are just never coupled to begin with.
//
// Generic and reusable by any monster that wants a "flies and spins" look
// with a different texture/duration/size/spin-speed - only a monster
// needing a genuinely different mechanism (e.g. a reveal-in-place effect,
// which SplashAttackAnimation already covers) needs a new AttackAnimation
// subclass.
class SpinningProjectileAnimation : public AttackAnimation
{
public:
    // `size` is the desired on-screen max dimension (in pixels) of the
    // projectile, independent of travel distance - derive it from the
    // project's existing on-board sizing (e.g. Config::MONSTER_BOARD_SIZE)
    // rather than the texture's native pixel size, same reasoning as
    // SplashAttackAnimation's `thickness` parameter.
    // `spinSpeedDegreesPerSecond` controls how fast the projectile visibly
    // rotates while flying; the default is a fast, clearly-visible spin.
    SpinningProjectileAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
        float duration, float size, float spinSpeedDegreesPerSecond = 720.f);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_origin;
    sf::Vector2f m_target;
    float m_duration;
    float m_elapsed = 0.f;
    float m_spinSpeedDegreesPerSecond;
};
