#pragma once
#include "State/State.h"
#include "Menu.h"
#include "Button.h"

// Full-screen "How To Play" screen: three pages (Instructions1/2/3.png)
// with Next/Previous buttons. BackButton/ESC exit to whichever state pushed
// this one; Next/Previous only ever change m_page, never the state stack.
class InstructionsState : public State
{
public:
    explicit InstructionsState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override { return MusicTrack::Menu; }

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

    int m_page = 0; // which of the 3 pages is showing, starts at 0

    // Shown/handled conditionally in draw()/handleEvent() based on m_page,
    // not an enabled/visible flag on Button itself.
    Button m_nextPageButton;
    Button m_prevPageButton;
};
