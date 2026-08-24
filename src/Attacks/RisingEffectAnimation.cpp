#include "Attacks/RisingEffectAnimation.h"
#include <algorithm>
#include <cstddef>

RisingEffectAnimation::RisingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
    float riseDistance, float duration, float size,
    int instanceCount, float horizontalSpacing, float staggerDelay)
    : m_riseDistance(riseDistance), m_duration(duration)
{
    sf::Vector2u textureSize = texture.getSize();
    float maxTextureDim = std::max(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
    float scale = (maxTextureDim > 0.f) ? (size / maxTextureDim) : 1.f;

    int count = std::max(instanceCount, 1);
    m_risers.reserve(static_cast<std::size_t>(count));

    // Centered horizontal spread: e.g. 3 instances -> offsets
    // [-spacing, 0, +spacing] around `position`; 1 instance -> offset 0,
    // identical to the original single-sprite placement.
    float firstOffset = -horizontalSpacing * static_cast<float>(count - 1) / 2.f;

    for (int i = 0; i < count; ++i)
    {
        Riser r{ sf::Sprite(texture) };
        r.sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });
        r.sprite.setScale({ scale, scale });
        r.bottomPosition = position + sf::Vector2f(firstOffset + horizontalSpacing * static_cast<float>(i), 0.f);
        r.sprite.setPosition(r.bottomPosition);
        r.startDelay = staggerDelay * static_cast<float>(i); // staggered start, not staggered finish
        m_risers.push_back(std::move(r));
    }
}

void RisingEffectAnimation::update(float dt)
{
    if (isFinished()) return;

    m_totalElapsed += dt;

    for (auto& r : m_risers)
    {
        if (r.finished) continue;

        if (!r.started)
        {
            if (m_totalElapsed < r.startDelay) continue; // not this instance's turn yet
            r.started = true;
        }

        r.elapsed += dt;
        float progress = (m_duration > 0.f) ? std::clamp(r.elapsed / m_duration, 0.f, 1.f) : 1.f;

        // Straight vertical rise - screen-space "up" is simply a smaller y, no
        // direction/rotation math needed since this never travels sideways.
        r.sprite.setPosition({ r.bottomPosition.x, r.bottomPosition.y - m_riseDistance * progress });

        if (progress >= 1.f)
        {
            r.finished = true;
            ++m_finishedCount;
        }
    }

    // Every instance has completed its rise - fire the shared impact
    // callback exactly once (e.g. the actual heal), regardless of how many
    // instances are rising together.
    if (m_finishedCount >= static_cast<int>(m_risers.size()))
    {
        fireImpact();
        finish();
    }
}

void RisingEffectAnimation::draw(sf::RenderWindow& window) const
{
    for (auto const& r : m_risers)
    {
        if (r.started)
            window.draw(r.sprite);
    }
}
