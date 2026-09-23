#pragma once
#include "State/State.h"
#include "Menu.h"

// Game-mode selection screen pushed by MenuState's Start Game button: pick
// Friend (local 2-player), AI, or Remote (LAN, via NetworkLobbyState), or
// go Back. Contains no game logic itself.
class GameModeState : public State
{
public:
    explicit GameModeState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override { return MusicTrack::Menu; }

private:
    void scaleBackgroundToWindow();
    void buildMenu();

    void onFriendClicked();
    void onAiClicked();
    void onRemoteClicked();
    void onBackClicked();

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu;
};
