#pragma once
#include "Attacks/AttackAnimation.h"

// A visual effect that "forms" in place by revealing a sprite top-to-bottom
// via a growing texture-rect clip, then holds before finishing. Used e.g.
// by Mozzy's freeze effect. Position never changes - only how much of the
// sprite is visible does.
class FormingEffectAnimation : public AttackAnimation
{
public:
    FormingEffectAnimation(const sf::Texture& texture, sf::Vector2f targetPosition,
        float width, float revealDuration, float holdDuration);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

private:
    sf::Sprite m_sprite;
    sf::Vector2u m_textureSize;
    float m_revealDuration;
    float m_holdDuration;
    float m_elapsed = 0.f;
    bool m_revealComplete = false;
};
