#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

// Abstract base only - not used directly by any monster. Every concrete
// animation below (and its per-monster usage) derives from this.

// Base class for a monster attack's visual effect: something that plays for
// a short time between "attack initiated" and "damage applied", then goes
// away. Board owns/updates/draws these polymorphically through this
// interface and never needs to know which monster - or which concrete
// animation - is playing; it only asks BoardEntity::createAttackAnimation()
// for one and reacts to isFinished().
//
// Deliberately knows nothing about Tile, HP, or combat rules - it is purely
// visual/timing. The actual damage is applied by whatever callback the
// caller (Board) wires up via setOnImpact(), fired exactly once, at the
// moment a concrete animation decides its effect has "reached" the target.
class AttackAnimation
{
public:
    virtual ~AttackAnimation() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

    // True once this animation is done and Board can discard it.
    bool isFinished() const { return m_finished; }

    // Set by the caller (Board) before the animation starts updating -
    // called exactly once, at impact.
    void setOnImpact(std::function<void()> onImpact) { m_onImpact = std::move(onImpact); }

protected:
    // Concrete animations call this exactly once, at the moment their
    // effect reaches the target - fires the caller's damage callback.
    void fireImpact()
    {
        if (m_impactFired) return;
        m_impactFired = true;
        if (m_onImpact) m_onImpact();
    }

    // Concrete animations call this once their visual is done playing
    // (typically right after fireImpact()).
    void finish() { m_finished = true; }

private:
    std::function<void()> m_onImpact;
    bool m_impactFired = false;
    bool m_finished = false;
};
