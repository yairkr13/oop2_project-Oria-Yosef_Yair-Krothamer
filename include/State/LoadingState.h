#pragma once
#include "State/State.h"

// Shown while the rest of the game's assets load. Controller preloads only
// this state's own background + spinner; update() drains the remaining
// asset queue a bit each frame so the spinner keeps animating instead of
// the game blocking on one big load.
class LoadingState : public State
{
public:
    explicit LoadingState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;

private:
    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    sf::Sprite m_spinner;
};
