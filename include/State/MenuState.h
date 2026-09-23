#pragma once
#include "State/State.h"
#include "Menu.h"
//#include "MusicToggleButton.h"
#include "AudioToggleButton.h"

// The game's main menu: background plus Start Game / Instructions / Exit
// buttons. Pure UI - contains no game logic itself.
class MenuState : public State
{
public:
    explicit MenuState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override;

private:
    void scaleBackgroundToWindow();
    void buildMenu();

    void onStartGameClicked();
    void onInstructionsClicked();
    void onExitClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;

    MusicToggleButton m_volumeButton; // fixed icon, not part of m_menu
    SoundToggleButton m_soundOnButton; // fixed icon, not part of m_menu
};
