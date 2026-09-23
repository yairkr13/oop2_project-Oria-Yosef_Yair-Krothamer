#pragma once
#include "State/State.h"
#include "Menu.h"
#include "Button.h"

// Full-screen "How To Play" screen - three pages (Instructions1/2/3.png)
// with Next/Previous buttons to move between them. Retrieves its assets
// from AssetsManager and contains no game logic. BackButton/ESC always exit
// this state entirely, back to whichever state pushed it (MenuState or
// MiniMenuState) - it has no knowledge of that state's type. Next/Previous
// only ever change which page is showing (m_page) - they never touch the
// state stack, so they stay completely separate from the exit path.
class InstructionsState : public State
{
public:
    explicit InstructionsState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override;

private:
    void scaleBackgroundToWindow();
    void buildMenu();

    void onBackClicked();

    // Switches m_background to page `page`'s own texture, clamped to [0, 2].
    void setPage(int page);
    void goToNextPage();
    void goToPreviousPage();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;

    // Which of the 3 instruction pages is currently showing - always starts
    // on page 0 (Instructions1.png).
    int m_page = 0;

    // Page-to-page navigation only (see setPage/goToNextPage/goToPreviousPage
    // above) - shown/handled conditionally in draw()/handleEvent() based on
    // m_page, rather than any enabled/visible flag on Button itself.
    Button m_nextPageButton;
    Button m_prevPageButton;
};
