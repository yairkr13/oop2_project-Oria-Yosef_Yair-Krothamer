#pragma once
#include "State/State.h"
#include "Menu.h"
#include "MusicToggleButton.h"

// The game's main menu: background image plus Start Game / Instructions / Exit
// buttons. Retrieves its assets from AssetsManager (loaded up front by
// Controller) and contains no game logic - Start Game and Instructions are
// wired to their own handlers so hooking up the real next states later is a
// one-line change inside those handlers.
class MenuState : public State
{
public:
    explicit MenuState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override { return MusicTrack::Menu; }

private:
    void scaleBackgroundToWindow();
    void buildMenu();

    void onStartGameClicked();
    void onInstructionsClicked();
    void onExitClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;

    // Not part of m_menu: a single fixed-position icon, not a stacked list.
    MusicToggleButton m_volumeButton;
};
