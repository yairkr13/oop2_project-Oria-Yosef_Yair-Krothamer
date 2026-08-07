#pragma once
#include "Button.h"

// The mute/unmute icon button, shown wherever a screen wants one (MenuState,
// MiniMenuState, ...). Reads and writes MusicManager's mute flag - the
// single source of truth - so every instance of this button always agrees
// with actual playback and with every other instance, with no per-screen
// mute state to keep in sync.
//
// draw() re-reads MusicManager and refreshes the icon every call, rather
// than only right after a click. Controller::run() draws every State in
// the stack every frame, including ones paused underneath others (e.g.
// MenuState sitting beneath GameplayState), so this is what keeps a
// button on a dormant, never-reconstructed State from showing a stale
// icon the next time it becomes visible - see the sync bug this replaced.
// m_button is mutable because that refresh is a visual sync, not a change
// to this object's own state, so draw() can stay const like every other
// State::draw().
class MusicToggleButton
{
public:
    // `position` is the button's top-left corner, `width` its on-screen
    // width - same convention as every other icon Button in the project.
    // Height follows from the texture's aspect ratio.
    MusicToggleButton(sf::Vector2i position, unsigned int width);

    void draw(sf::RenderWindow& window) const;
    void handleEvent(const sf::Event& event);

private:
    void onClicked();
    void refreshTexture() const;

    mutable Button m_button;
};
