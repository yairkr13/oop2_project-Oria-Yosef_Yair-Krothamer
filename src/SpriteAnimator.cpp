#include "SpriteAnimator.h"
#include <algorithm>

void SpriteAnimator::addState(int id, std::unique_ptr<SpriteSheet> sheet,
    std::function<bool()> isActive, int priority)
{
    Entry entry{ id, std::move(sheet), std::move(isActive), priority };

    // Insert keeping m_states sorted by ascending priority, so update()'s
    // scan below can just take the first match - registration order is
    // never significant (see addState's own comment in the header).
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

    // Advance only the newly-active state's own clock; reset every other
    // one back to frame 0 so it starts fresh whenever it next becomes
    // active - exactly the active/reset split Monster used to do by hand
    // for each of its four sheets individually.
    for (std::size_t i = 0; i < m_states.size(); ++i)
    {
        if (i == newActive)
            m_states[i].sheet->update(dt);
        else
            m_states[i].sheet->reset();
    }

    m_activeIndex = newActive;
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

int SpriteAnimator::getActiveStateId() const
{
    return hasActiveState() ? m_states[m_activeIndex].id : -1;
}

bool SpriteAnimator::isStateFinished(int id) const
{
    for (auto const& entry : m_states)
    {
        if (entry.id == id)
            return entry.sheet->isFinished();
    }
    return true; // never registered - nothing to wait for
}
