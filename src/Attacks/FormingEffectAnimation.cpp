#include "Attacks/FormingEffectAnimation.h"
#include <algorithm>

FormingEffectAnimation::FormingEffectAnimation(const sf::Texture& texture, sf::Vector2f targetPosition,
    float width, float revealDuration, float holdDuration)
    : m_sprite(texture), m_textureSize(texture.getSize()), m_revealDuration(revealDuration), m_holdDuration(holdDuration)
{
    // Scale off width alone (not the texture's largest dimension) so a
    // tall/narrow source image still ends up the intended width on screen -
    // height simply follows from the texture's own aspect ratio.
    float scale = (m_textureSize.x > 0) ? (width / static_cast<float>(m_textureSize.x)) : 1.f;
    float scaledHeight = static_cast<float>(m_textureSize.y) * scale;

    // The reveal's fixed top point, derived from the ACTUAL on-screen
    // height computed above - never assumed - so the fully-revealed image
    // (bottom edge at topPosition.y + scaledHeight) ends up correctly
    // centered on targetPosition regardless of the texture's proportions.
    sf::Vector2f topPosition = targetPosition - sf::Vector2f(0.f, scaledHeight / 2.f);

    // Anchor at the top-center of the texture (not its middle) so the
    // sprite's fixed screen position IS the top of the reveal - clipping the
    // texture-rect's height then grows the visible area straight down from
    // that same fixed point, without ever moving the sprite itself.
    m_sprite.setOrigin({ static_cast<float>(m_textureSize.x) / 2.f, 0.f });
    m_sprite.setPosition(topPosition);
    m_sprite.setScale({ scale, scale });

    // Start fully un-revealed - only a thin sliver grows in from here.
    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(m_textureSize.x), 0 }));
}

void FormingEffectAnimation::update(float dt)
{
    if (isFinished()) return;

    m_elapsed += dt;

    if (!m_revealComplete)
    {
        float progress = (m_revealDuration > 0.f) ? std::clamp(m_elapsed / m_revealDuration, 0.f, 1.f) : 1.f;
        int revealedHeight = static_cast<int>(static_cast<float>(m_textureSize.y) * progress);
        m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(m_textureSize.x), revealedHeight }));

        if (progress >= 1.f)
        {
            m_revealComplete = true;
            m_elapsed = 0.f; // restart the clock for the hold phase below
            fireImpact(); // the effect has visually "landed" exactly when the reveal completes
        }
        return;
    }

    // Fully formed - hold as-is until holdDuration has elapsed, then let
    // Board/Monster discard us like any other finished animation.
    if (m_elapsed >= m_holdDuration)
        finish();
}

void FormingEffectAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
