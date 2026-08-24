#pragma once
#include "Attacks/AttackAnimation.h"

// Used by Mozzy for its special ability (the FreezeEffect that forms over
// the frozen target).

// An effect that gradually "forms" over its target in place: a sprite
// anchored at a fixed position that reveals top-to-bottom via a growing
// texture-rect clip (no sideways movement, no attacker->target travel),
// then holds fully visible for a moment before finishing.
//
// Conceptually the vertical, in-place sibling of SplashAttackAnimation's
// clip-reveal - the same "texture-rect grows, sprite position never moves"
// trick, just revealing downward from a fixed top position instead of
// outward from an attacker toward a target. Distinct from
// RisingEffectAnimation, which instead translates a fully-visible sprite
// upward - here the sprite never moves, only how much of it is visible
// changes, which is what makes something look like it's forming/growing
// rather than flying in.
//
// Deliberately knows nothing about Mozzy, freezing, HP, or which entity
// it's attached to - it only reveals a sprite downward and reports when
// it's done, exactly like its AttackAnimation siblings. Generic and
// reusable by any future Special wanting this "materializes from above"
// look, with a different texture/duration/size/hold time.
class FormingEffectAnimation : public AttackAnimation
{
public:
    // `targetPosition` is the point the FULLY-REVEALED image should end up
    // centered on (e.g. the target monster's own screen position) - the
    // class derives where the reveal starts (above that point) from the
    // image's own actual on-screen height once scaled, not from a value the
    // caller has to predict, so centering stays correct regardless of the
    // texture's aspect ratio. `width`: desired on-screen width of the
    // effect, independent of texture resolution - deliberately drives only
    // the horizontal axis (rather than the texture's largest dimension,
    // like most of this project's other effects use) so a tall/narrow
    // source image still ends up as wide as intended; height then follows
    // from the texture's own aspect ratio, exactly like
    // SplashAttackAnimation already scales its reach and thickness on two
    // independent axes for its own unrelated reasons. `revealDuration`:
    // seconds for the top-to-bottom reveal. `holdDuration`: seconds the
    // fully-formed image stays visible once revealed, before this
    // animation reports finished.
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
