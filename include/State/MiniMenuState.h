#pragma once
#include "State/State.h"
#include "Menu.h"
//#include "MusicToggleButton.h"
#include "AudioToggleButton.h"
#include <functional>

// Pause overlay pushed on top of GameplayState (Escape, or the on-board
// button). Resume/Instructions are self-contained; Restart/Exit need to
// finish the paused GameplayState beneath this one, so GameplayState hands
// in callbacks for those two at push time instead of this state reaching in.
class MiniMenuState : public State
{
public:
    MiniMenuState(sf::RenderWindow& window, std::function<void()> onExitGame, std::function<void()> onRestartGame);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;

private:
    void scaleAndCenterBackground();
    void buildMenu();

    void onResumeClicked();
    void onInstructionsClicked();
    void onRestartClicked();
    void onExitClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;

    MusicToggleButton m_volumeButton; // fixed icon, not part of m_menu
    SoundToggleButton m_soundOnButton; // fixed icon, not part of m_menu

    std::function<void()> m_onExitGame;
    std::function<void()> m_onRestartGame; // may be empty for a PlayerVsRemote match
};
