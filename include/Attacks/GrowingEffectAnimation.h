#pragma once
#include "Attacks/AttackAnimation.h"

// Used by Henrietta (special ability, the ShieldEffect for Protection) and
// Barzilla (special ability, the impact flash on an Empowered Attack hit).

// An effect that grows outward from its own center over a fixed position,
// holds briefly at full size, then optionally fades out - the shape a
// "barrier forming around a target" look needs (e.g. Henrietta's
// Protection shield).
//
// Distinct from every other AttackAnimation subclass: SplashAttackAnimation/
// FormingEffectAnimation reveal via a growing texture-rect clip (the source
// image's pixels are static, only how much of them is visible changes);
// RisingEffectAnimation translates a fully-visible sprite; this one instead
// scales a fully-visible sprite up from nothing, anchored at its own
// center throughout, so it visibly expands around a fixed point rather
// than growing toward/away from an edge.
//
// Deliberately knows nothing about Henrietta, Protection, or which entity
// it's attached to - it only grows/holds/fades a sprite in place and
// reports when it's done, exactly like its AttackAnimation siblings.
// Generic and reusable by any future Special wanting this "expands around
// a point" look, with a different texture/size/timing.
class GrowingEffectAnimation : public AttackAnimation
{
public:
    // `position`: the fixed center point the effect grows around and stays
    // centered on - e.g. the target's own screen position. `size`: desired
    // on-screen max dimension at full size, independent of texture
    // resolution - same convention as the other AttackAnimation subclasses.
    // `growDuration`: seconds to scale up from nothing to full size.
    // `holdDuration`: seconds to stay at full size once grown.
    // `fadeDuration`: seconds to fade out (alpha) before finishing; 0 (the
    // default) skips the fade and finishes immediately once the hold ends.
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
    float m_fadeDuration;
    float m_elapsed = 0.f;
    Phase m_phase = Phase::Growing;
};
