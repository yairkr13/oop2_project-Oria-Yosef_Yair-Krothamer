#pragma once
#include "Attacks/AttackAnimation.h"

// A visual effect that grows outward from its own center, holds at full
// size, then optionally fades out - e.g. Henrietta's Protection shield or
// Barzilla's empowered-attack impact flash.
class GrowingEffectAnimation : public AttackAnimation
{
public:
    GrowingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
        float size, float growDuration, float holdDuration, float fadeDuration = 0.f);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

private:
    enum class Phase { Growing, Holding, FadingOut };

    sf::Sprite m_sprite;
    float m_fullScale;
    float m_growDuration;
    float m_holdDuration;
    float m_fadeDuration; // 0 skips the fade phase
    float m_elapsed = 0.f;
    Phase m_phase = Phase::Growing;
};
