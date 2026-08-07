#pragma once
#include "State/State.h"
#include "Menu.h"

// Game-mode selection screen pushed by MenuState's Start Game button. Same
// background as the main menu; lets the player pick Friend (local 2-player)
// or AI, or go Back to whichever state pushed this one. Retrieves its assets
// from AssetsManager and contains no game logic - Friend/AI are wired to
// their own handlers so hooking up the real gameplay state later is a
// one-line change inside those handlers.
class GameModeState : public State
{
public:
    explicit GameModeState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    bool usesMenuMusic() const override { return true; }

private:
    void scaleBackgroundToWindow();
    void buildMenu();

    void onFriendClicked();
    void onAiClicked();
    void onBackClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;
};
