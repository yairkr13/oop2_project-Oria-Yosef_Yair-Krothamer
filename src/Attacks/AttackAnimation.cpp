#include "Attacks/AttackAnimation.h"

bool AttackAnimation::isFinished() const
{
    return m_finished;
}

void AttackAnimation::setOnImpact(std::function<void()> onImpact)
{
    m_onImpact = std::move(onImpact);
}

// Fires the impact callback exactly once per animation.
void AttackAnimation::fireImpact()
{
    if (m_impactFired) return;
    m_impactFired = true;
    if (m_onImpact) m_onImpact();
}

void AttackAnimation::finish()
{
    m_finished = true;
}
