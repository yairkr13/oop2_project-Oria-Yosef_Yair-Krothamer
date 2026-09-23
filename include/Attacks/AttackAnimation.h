#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

// Abstract base for a monster attack's visual effect: plays between "attack
// initiated" and "damage applied", then finishes. Board owns/updates/draws
// these polymorphically and never needs to know which concrete animation is
// playing.
class AttackAnimation
{
public:
    virtual ~AttackAnimation() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

    bool isFinished() const;
    void setOnImpact(std::function<void()> onImpact);

protected:
    void fireImpact();
    void finish();

private:
    std::function<void()> m_onImpact;
    bool m_impactFired = false;
    bool m_finished = false;
};
