#pragma once
#include "State/State.h"
#include "Menu.h"
#include "Button.h"
#include <functional>
#include <optional>

// Pause-style overlay pushed on top of GameplayState (Escape, or the
// on-board button). Renders as a small popup centered over the window -
// GameplayState stays visible underneath since Controller draws the whole
// state stack, not just the top.
//
// Resume and Instructions are fully self-contained (bare transitionTo() /
// pushState(InstructionsState), same as every other back-navigation in the
// project). Restart and Exit need to also finish the paused GameplayState
// beneath this one, which this state has no access to and must not reach
// into - GameplayState hands in callbacks for those two at push time.
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
    void buildVolumeButton();

    void onResumeClicked();
    void onInstructionsClicked();
    void onRestartClicked();
    void onExitClicked();
    void onVolumeClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;

    // Not part of m_menu: a single fixed-position icon, not a stacked list.
    std::optional<Button> m_volumeButton;
    bool m_musicMuted = false;

    std::function<void()> m_onExitGame;
    std::function<void()> m_onRestartGame;
};
