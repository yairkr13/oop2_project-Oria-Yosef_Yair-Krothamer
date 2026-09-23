#include "SpriteAnimator.h"
#include <algorithm>

// Keeps m_states sorted by ascending priority so update()'s scan can just
// take the first match.
void SpriteAnimator::addState(int id, std::unique_ptr<SpriteSheet> sheet,
    std::function<bool()> isActive, int priority)
{
    Entry entry{ id, std::move(sheet), std::move(isActive), priority };

    auto pos = std::find_if(m_states.begin(), m_states.end(),
        [priority](const Entry& existing) { return existing.priority > priority; });
    m_states.insert(pos, std::move(entry));
}

void SpriteAnimator::update(float dt)
{
    std::size_t newActive = npos;
    for (std::size_t i = 0; i < m_states.size(); ++i)
    {
        if (m_states[i].isActive())
        {
            newActive = i; // first match = lowest priority among the true ones
            break;
        }
    }

    // Advance only the active state's clock; reset every other one so it
    // starts fresh whenever it next becomes active.
    for (std::size_t i = 0; i < m_states.size(); ++i)
    {
        if (i == newActive)
            m_states[i].sheet->update(dt);
        else
            m_states[i].sheet->reset();
    }

    m_activeIndex = newActive;
}

bool SpriteAnimator::hasActiveState() const
{
    return m_activeIndex != npos;
}

bool SpriteAnimator::hasAnyState() const
{
    return !m_states.empty();
}

void SpriteAnimator::applyCurrentFrame(sf::Sprite& sprite) const
{
    if (!hasActiveState()) return;
    m_states[m_activeIndex].sheet->applyCurrentFrame(sprite);
}

float SpriteAnimator::getActiveBaseScale() const
{
    return hasActiveState() ? m_states[m_activeIndex].sheet->getBaseScale() : 1.f;
}

// Returns true (nothing to wait for) if `id` was never registered.
bool SpriteAnimator::isStateFinished(int id) const
{
    for (auto const& entry : m_states)
    {
        if (entry.id == id)
            return entry.sheet->isFinished();
    }
    return true;
}
