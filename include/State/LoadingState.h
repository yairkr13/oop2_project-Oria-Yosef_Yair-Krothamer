#pragma once
#include "State/State.h"

// Shown for the brief window between the game launching and the rest of its
// assets (menu/button textures, fonts, music, every monster's sprite sheets,
// ...) finishing loading. Its own background + spinner are the only two
// assets Controller loads before this state exists (see
// AssetsManager::loadBootAssets()) - everything else is queued
// (queueRemainingAssets()) and drained one asset per frame from update()
// below, so this state's draw() keeps running (and the spinner keeps
// turning) the whole time loading is happening, instead of the game
// blocking on one big load before the window ever shows a frame.
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
