#pragma once
#include "Attacks/StaggeredInstancesAnimation.h"

// One or more sprites, spread horizontally around a fixed point, that rise
// straight up in place and fire impact once all have finished. Used by
// Muffintop's heal effect - plays on a single target, not between two.
class RisingEffectAnimation : public StaggeredInstancesAnimation
{
public:
    RisingEffectAnimation(const sf::Texture& texture, sf::Vector2f position,
        float riseDistance, float duration, float size,
        int instanceCount = 1, float horizontalSpacing = 0.f, float staggerDelay = 0.f);

private:
    void positionInstance(Instance& instance, float progress) override;

    float m_riseDistance;
};
