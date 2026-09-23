#pragma once
#include "SpriteSheet.h"
#include <functional>
#include <memory>
#include <vector>

// Holds several named SpriteSheet states and decides, each frame, which one
// is currently active (by priority, among those whose predicate is true).
// Knows nothing about gameplay - the owner (e.g. Monster) defines what each
// state id and predicate means.
class SpriteAnimator
{
public:
    void addState(int id, std::unique_ptr<SpriteSheet> sheet,
        std::function<bool()> isActive, int priority);

    void update(float dt);
    bool hasActiveState() const;
    bool hasAnyState() const;
    void applyCurrentFrame(sf::Sprite& sprite) const;
    float getActiveBaseScale() const;
    bool isStateFinished(int id) const;

private:
    struct Entry
    {
        int id;
        std::unique_ptr<SpriteSheet> sheet;
        std::function<bool()> isActive;
        int priority; // lowest value wins when multiple predicates are true
    };

    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    std::vector<Entry> m_states; // kept sorted by ascending priority
    std::size_t m_activeIndex = npos;
};
