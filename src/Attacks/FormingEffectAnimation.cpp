#include "Attacks/FormingEffectAnimation.h"
#include <algorithm>

FormingEffectAnimation::FormingEffectAnimation(const sf::Texture& texture, sf::Vector2f targetPosition,
    float width, float revealDuration, float holdDuration)
    : m_sprite(texture), m_textureSize(texture.getSize()), m_revealDuration(revealDuration), m_holdDuration(holdDuration)
{
    // Scale by width only, so a tall/narrow texture still ends up the
    // intended on-screen width; height follows from its aspect ratio.
    float scale = (m_textureSize.x > 0) ? (width / static_cast<float>(m_textureSize.x)) : 1.f;
    float scaledHeight = static_cast<float>(m_textureSize.y) * scale;

    // Fixed top point derived from the actual scaled height, so the fully
    // revealed image ends up centered on targetPosition.
    sf::Vector2f topPosition = targetPosition - sf::Vector2f(0.f, scaledHeight / 2.f);

    // Anchor at top-center so clipping the texture-rect grows the reveal
    // straight down without moving the sprite itself.
    m_sprite.setOrigin({ static_cast<float>(m_textureSize.x) / 2.f, 0.f });
    m_sprite.setPosition(topPosition);
    m_sprite.setScale({ scale, scale });

    // Start fully un-revealed.
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
            m_elapsed = 0.f; // restart clock for the hold phase
            fireImpact(); // effect "lands" exactly when the reveal completes
        }
        return;
    }

    // Fully formed - hold until holdDuration elapses, then finish.
    if (m_elapsed >= m_holdDuration)
        finish();
}

void FormingEffectAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
