#include "Attacks/SplashAttackAnimation.h"
#include <cmath>
#include <algorithm>

SplashAttackAnimation::SplashAttackAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
    float duration, float thickness)
    : m_sprite(texture), m_textureSize(texture.getSize()), m_duration(duration)
{
    // Anchor at the emitting edge of the texture (left-center) rather than
    // its center, so the image sits *at* the attacker and grows away from
    // it, instead of being centered on the attacker.
    m_sprite.setOrigin({ 0.f, static_cast<float>(m_textureSize.y) / 2.f });
    m_sprite.setPosition(origin);

    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Rotating the sprite (rather than assuming a fixed screen-space
    // direction) is what makes this work for every attack direction: the
    // texture-rect reveal in update() always clips along the sprite's own
    // local X axis, and this rotation carries that axis to point at the
    // target regardless of where the target is on screen.
    m_sprite.setRotation(sf::radians(std::atan2(dy, dx)));

    // X (reach) and Y (thickness) are scaled completely independently, from
    // two unrelated quantities, so resizing one can never affect the other:
    //  - scaleX stretches the local width to exactly span the
    //    attacker->target distance, so revealing texture width always ==
    //    revealing screen-space distance toward the target, regardless of
    //    how big the source image is.
    //  - scaleY maps the texture's native pixel height down to the
    //    caller-specified on-screen `thickness`, independent of distance -
    //    this is what keeps the splash's visual weight proportional to the
    //    board/monsters instead of ballooning to the raw asset's size.
    float scaleX = (m_textureSize.x > 0) ? (distance / static_cast<float>(m_textureSize.x)) : 1.f;
    float scaleY = (m_textureSize.y > 0) ? (thickness / static_cast<float>(m_textureSize.y)) : 1.f;
    m_sprite.setScale({ scaleX, scaleY });

    // Start fully un-revealed.
    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 0, static_cast<int>(m_textureSize.y) }));
}

void SplashAttackAnimation::update(float dt)
{
    if (isFinished()) return;

    m_elapsed += dt;
    float progress = (m_duration > 0.f) ? std::clamp(m_elapsed / m_duration, 0.f, 1.f) : 1.f;

    int revealedWidth = static_cast<int>(static_cast<float>(m_textureSize.x) * progress);
    m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { revealedWidth, static_cast<int>(m_textureSize.y) }));

    if (progress >= 1.f)
    {
        fireImpact(); // damage applies exactly when the reveal reaches the target
        finish();     // Board discards us next frame
    }
}

void SplashAttackAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
