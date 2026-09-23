#pragma once
#include "Attacks/AttackAnimation.h"

// A "splash" attack: a sprite anchored at the attacker, rotated to face the
// target, that reveals from attacker toward target over its duration, then
// fires impact on arrival. Works for any of the six board directions since
// the sprite itself is rotated to face the target.
class SplashAttackAnimation : public AttackAnimation
{
public:
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
