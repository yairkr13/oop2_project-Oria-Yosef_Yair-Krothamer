#pragma once
#include "Attacks/AttackAnimation.h"
#include <vector>
#include <functional>

// Template-method base for an AttackAnimation that plays N staggered copies
// of one texture, firing the shared impact callback once every instance has
// finished. Shared by BurstProjectileAnimation and RisingEffectAnimation -
// only how a single instance moves (positionInstance) differs per subclass.
class StaggeredInstancesAnimation : public AttackAnimation
{
public:
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

protected:
    struct Instance
    {
        sf::Sprite sprite;
        sf::Vector2f anchor; // this instance's own fixed reference point
        float startDelay;
        float elapsed = 0.f;
        bool started = false;
        bool finished = false;
    };

    // Builds `count` instances of `texture`, scaled to `size` and launched
    // `staggerInterval` seconds apart. `anchorForIndex` supplies each
    // instance's own fixed reference point.
    void initInstances(const sf::Texture& texture, int count, float staggerInterval,
        float size, float instanceDuration, const std::function<sf::Vector2f(int)>& anchorForIndex);

    // Repositions `instance` for this frame given its clamped [0,1]
    // progress. The one step subclasses must implement themselves.
    virtual void positionInstance(Instance& instance, float progress) = 0;

    std::vector<Instance> m_instances;

private:
    float m_instanceDuration = 0.f;
    float m_totalElapsed = 0.f;
    int m_finishedCount = 0;
};
