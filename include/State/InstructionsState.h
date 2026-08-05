#pragma once
#include "State/State.h"
#include "Menu.h"

// Full-screen "How To Play" screen. Retrieves its assets from AssetsManager
// and contains no game logic. Its only interactive element is a Back button
// that pops back to whichever state pushed it (MenuState or MiniMenuState) -
// it has no knowledge of that state's type.
class InstructionsState : public State
{
public:
    explicit InstructionsState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;

private:
    void scaleBackgroundToWindow();
    void buildMenu();

    void onBackClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;
};
