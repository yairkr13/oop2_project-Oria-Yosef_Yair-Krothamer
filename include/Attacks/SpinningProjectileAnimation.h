#pragma once
#include "Attacks/AttackAnimation.h"

// A projectile that flies in a straight line from attacker to target while
// spinning continuously, firing impact the instant it arrives. Position and
// spin are independent, so it looks correct from any attack direction.
class SpinningProjectileAnimation : public AttackAnimation
{
public:
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
