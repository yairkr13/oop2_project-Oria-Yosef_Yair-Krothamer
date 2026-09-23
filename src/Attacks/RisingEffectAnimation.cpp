#include "Attacks/RisingEffectAnimation.h"
#include <algorithm>

RisingEffectAnimation::RisingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
    float riseDistance, float duration, float size,
    int instanceCount, float horizontalSpacing, float staggerDelay)
    : m_riseDistance(riseDistance)
{
    int count = std::max(instanceCount, 1);

    // Centered spread, e.g. 3 instances -> offsets [-spacing, 0, +spacing].
    float firstOffset = -horizontalSpacing * static_cast<float>(count - 1) / 2.f;

    initInstances(texture, count, staggerDelay, size, duration,
        [position, firstOffset, horizontalSpacing](int i) {
            return position + sf::Vector2f(firstOffset + horizontalSpacing * static_cast<float>(i), 0.f);
        });
}

void RisingEffectAnimation::positionInstance(Instance& instance, float progress)
{
    instance.sprite.setPosition({ instance.anchor.x, instance.anchor.y - m_riseDistance * progress });
}
