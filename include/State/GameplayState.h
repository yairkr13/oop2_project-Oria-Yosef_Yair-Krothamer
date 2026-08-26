#pragma once
#include "State/State.h"
#include "Constants.h"
#include "Board.h"
#include "Player.h"
#include "TurnManager.h"
#include "Button.h"
#include <optional>

class Card;
class Monster;
class BoardEntity;

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
    //Monster* m_selectedFromHand = nullptr;

    Card* m_selectedFromHand = nullptr;   // קלף שנבחר ליד, טרם שוחק - ממתין ל-tile להנחה
    Card* m_pendingSpecialCard = nullptr; // קלף ששוחק, יכולת מיוחדת שלו ממתינה לבחירת מטרה

    // The on-board Monster/Heart currently selected, awaiting a second
    // click to move/attack with - the same kind of interaction/input state
    // m_selectedFromHand above already is, and owned the same way: a
    // non-owning pointer into whatever Player actually owns the Monster
    // (see Player::m_monsters). GameplayState never takes ownership here,
    // exactly like it never takes ownership of a selected Card.
    BoardEntity* m_selectedEntity = nullptr;

    std::optional<Button> m_miniMenuButton;

    // מטפל בלחיצה על קלף ששוחק כבר (מציג "READY"/"CD: X") - מפעיל מצב בחירת מטרה ליכולת
    void handleSpecialAbilityClick(Card* card);

    // מטפל בלחיצה על tile כשיש קלף נבחר להנחה - בודק תקינות לפני שקוראים ל-playCard
    void handleSpawnAttempt(const sf::Vector2f& pos, Player& current);

    // מטפל בלחיצה על tile כשיש יכולת מיוחדת ממתינה למטרה
    void handleSpecialTargetClick(const sf::Vector2f& pos);

    // Interprets a plain board click - the sequence "select an entity, then
    // click a highlighted tile to move/attack with it, or click elsewhere
    // to deselect." Formerly Board::handleClick(); moved here so all four
    // click-interpretation flows (this one, handleSpawnAttempt,
    // handleSpecialTargetClick, handleSpecialAbilityClick) live in one
    // place and share the same shape - Board now only exposes the
    // primitives (getTileAtScreenPosition/selectEntity/performAction/
    // clearHighlights) this calls, never the interaction decision itself.
    void handleBoardClick(const sf::Vector2f& pos, Player& current);

    // The one place m_pendingSpecialCard is abandoned before it reached its
    // own commit event (toggled off, superseded by a different selection,
    // or the turn ended). Un-arms whatever the pending Card's monster may
    // have armed (see Monster::cancelSpecialAbility - a no-op for any
    // Special that hadn't armed anything yet) before clearing the pointer,
    // so a Special can never stay silently armed once its Card stops
    // showing as selected.
    void clearPendingSpecial();

    // Highlights every currently-valid target Tile for `caster`'s pending
    // Special - derived fresh from Board::getOccupiedTiles() and
    // caster.isValidSpecialTarget() each time selection begins, not stored
    // anywhere new. Which Tiles qualify and what color they get are both
    // decided entirely by `caster` (see Monster::isValidSpecialTarget /
    // getSpecialTargetHighlightColor); this only ever iterates and paints.
    void highlightValidSpecialTargets(Monster& caster);

};
