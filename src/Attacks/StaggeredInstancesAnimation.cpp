#include "Attacks/StaggeredInstancesAnimation.h"
#include "SpriteUtils.h"
#include <algorithm>
#include <cstddef>

void StaggeredInstancesAnimation::initInstances(const sf::Texture& texture, int count, float staggerInterval,
    float size, float instanceDuration, const std::function<sf::Vector2f(int)>& anchorForIndex)
{
    m_instanceDuration = instanceDuration;

    sf::Vector2u textureSize = texture.getSize();
    float scale = SpriteUtils::maxDimensionScale(textureSize, size);

    int safeCount = std::max(count, 0);
    m_instances.reserve(static_cast<std::size_t>(safeCount));
    for (int i = 0; i < safeCount; ++i)
    {
        Instance inst{ sf::Sprite(texture) };
        inst.sprite.setOrigin({ static_cast<float>(textureSize.x) / 2.f, static_cast<float>(textureSize.y) / 2.f });
        inst.sprite.setScale({ scale, scale });
        inst.anchor = anchorForIndex(i);
        inst.sprite.setPosition(inst.anchor);
        inst.startDelay = staggerInterval * static_cast<float>(i); // staggered start, not staggered finish
        m_instances.push_back(std::move(inst));
    }
}

void StaggeredInstancesAnimation::update(float dt)
{
    if (isFinished()) return;

    m_totalElapsed += dt;

    for (auto& inst : m_instances)
    {
        if (inst.finished) continue;

        if (!inst.started)
        {
            if (m_totalElapsed < inst.startDelay) continue; // not this instance's turn yet
            inst.started = true;
        }

        inst.elapsed += dt;
        float progress = (m_instanceDuration > 0.f) ? std::clamp(inst.elapsed / m_instanceDuration, 0.f, 1.f) : 1.f;
        positionInstance(inst, progress);

        if (progress >= 1.f)
        {
            inst.finished = true;
            ++m_finishedCount;
        }
    }

    // Every instance has completed - fire the shared impact callback exactly
    // once, regardless of how many instances were launched.
    if (m_finishedCount >= static_cast<int>(m_instances.size()))
    {
        fireImpact();
        finish();
    }
}

void StaggeredInstancesAnimation::draw(sf::RenderWindow& window) const
{
    for (auto const& inst : m_instances)
        if (inst.started)
            window.draw(inst.sprite);
}
