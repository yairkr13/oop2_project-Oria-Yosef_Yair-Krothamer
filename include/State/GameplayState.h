#pragma once
#include "State/State.h"
#include "Constants.h"
#include "Board.h"
#include "Player.h"
#include "TurnManager.h"
#include "Button.h"
#include <optional>

// The active match screen: owns the board and both players for one game,
// renders them, drives the per-frame update loop, and routes gameplay
// input (board/hand clicks, Space to end turn, Escape to quit to menu).
class GameplayState : public State
{
public:
    GameplayState(sf::RenderWindow& window, GameMode mode);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override { return MusicTrack::Gameplay; }

private:
    void scaleBackgroundToWindow();

    // Builds player 2 as an AIPlayer or a plain Player depending on mode.
    // A helper function rather than a ternary in the member-initializer
    // list, since unique_ptr<AIPlayer>/unique_ptr<Player> don't resolve to
    // a common type there.
    static std::unique_ptr<Player> makePlayer2(GameMode mode);

    void buildMiniMenuButton();

    // Pushes MiniMenuState, wiring its Restart/Exit callbacks to finish this
    // (paused) state - see MiniMenuState for why those need a callback
    // rather than a stack operation. Shared by Escape and the on-board
    // button so the two triggers can't drift apart.
    void openMiniMenu();

    void handle(const sf::Event::MouseButtonPressed& event);
    void handle(const sf::Event::KeyPressed& event);
    void handle(const auto& event) {}

    sf::RenderWindow& m_window;
    sf::Sprite m_background;

    // Remembered only for Restart (see openMiniMenu) - nothing else in this
    // class ever branches on it.
    GameMode m_mode;

    // Declaration order matters here: m_board/m_player1/m_player2 must be
    // constructed before m_turnManager, since its constructor binds
    // references to them.
    Board m_board;
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    TurnManager m_turnManager;

    // Hand-selected monster awaiting placement. UI/gameplay-input state,
    // not board state.
    Monster* m_selectedFromHand = nullptr;

    std::optional<Button> m_miniMenuButton;
};
