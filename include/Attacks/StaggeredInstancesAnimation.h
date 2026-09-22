#pragma once
#include "Attacks/AttackAnimation.h"
#include <vector>
#include <functional>

// Template Method base for an AttackAnimation that plays N copies of one
// texture, each launched `staggerInterval` seconds after the previous one,
// animating independently until every instance has reached progress==1 -
// at which point the shared impact callback fires exactly once, regardless
// of how many instances there were.
//
// Shared by BurstProjectileAnimation (instances fly in a straight line from
// an origin to a target) and RisingEffectAnimation (instances rise straight
// up in place) - staggering, per-instance timing, counting how many have
// finished, and firing the impact once are identical between them and live
// here exactly once. The one thing that genuinely differs - HOW a single
// instance moves, given its own progress - stays virtual (positionInstance),
// left to each concrete subclass.
class StaggeredInstancesAnimation : public AttackAnimation
{
public:
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

protected:
    struct Instance
    {
        sf::Sprite sprite;
        sf::Vector2f anchor; // this instance's own fixed reference point - positionInstance() moves it relative to this
        float startDelay;
        float elapsed = 0.f;
        bool started = false;
        bool finished = false;
    };

    // Builds `count` instances of `texture` (0 or negative -> none), each
    // scaled to `size` (see SpriteUtils::maxDimensionScale) and launched
    // `staggerInterval` seconds apart. `anchorForIndex` supplies each
    // instance's own fixed reference point - constant for every index if
    // they all share one (BurstProjectileAnimation's single shared origin),
    // varying per index for something like RisingEffectAnimation's
    // horizontally-spread starting points. `instanceDuration` is how long,
    // in seconds, ONE instance takes to go from progress 0 to 1. Each
    // instance's sprite starts positioned at its own anchor - a subclass
    // whose instances need anything else set up front (e.g.
    // BurstProjectileAnimation's shared facing rotation) does that itself,
    // afterward, over the protected m_instances this leaves populated.
    void initInstances(const sf::Texture& texture, int count, float staggerInterval,
        float size, float instanceDuration, const std::function<sf::Vector2f(int)>& anchorForIndex);

    // Repositions `instance`'s sprite for this frame, given its own
    // clamped-to-[0,1] progress through its instanceDuration - called once
    // per active instance per frame, after `instance.elapsed` has already
    // been advanced. The one step every StaggeredInstancesAnimation needs
    // but can't share, since it's exactly what makes each concrete
    // animation look different.
    virtual void positionInstance(Instance& instance, float progress) = 0;

    std::vector<Instance> m_instances;

private:
    float m_instanceDuration = 0.f;
    float m_totalElapsed = 0.f;
    int m_finishedCount = 0;
};
