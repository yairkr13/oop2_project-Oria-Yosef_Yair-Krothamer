#include "Attacks/SplashAttackAnimation.h"
#include <cmath>
#include <algorithm>

SplashAttackAnimation::SplashAttackAnimation(const sf::Texture& texture, sf::Vector2f origin, sf::Vector2f target,
    float duration, float thickness)
    : m_sprite(texture), m_textureSize(texture.getSize()), m_duration(duration)
{
    // Anchor at the left-center edge so the sprite sits at the attacker and
    // grows away from it, rather than being centered on the attacker.
    m_sprite.setOrigin({ 0.f, static_cast<float>(m_textureSize.y) / 2.f });
    m_sprite.setPosition(origin);

    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Rotate to face the target so the local-X reveal in update() always
    // points the right way, regardless of attack direction.
    m_sprite.setRotation(sf::radians(std::atan2(dy, dx)));

    // X and Y scaled independently: X spans the attacker->target distance,
    // Y maps to the caller-specified on-screen thickness.
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
        finish();
    }
}

void SplashAttackAnimation::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}
