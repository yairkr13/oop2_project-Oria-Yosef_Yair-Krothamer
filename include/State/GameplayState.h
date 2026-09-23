#pragma once
#include "State/State.h"
#include "Constants.h"
#include "Board.h"
#include "Player.h"
#include "RemotePlayer.h"
#include "NetworkConnection.h"
#include "TurnManager.h"
#include "Button.h"
#include <optional>
#include "Tooltip.h"
#include "State/BottomPanel.h"

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

    GameplayState(sf::RenderWindow& window, std::unique_ptr<NetworkConnection> connection, PlayerSide localSide);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override { return MusicTrack::Gameplay; }

private:
    void clearSelectionState();

    void scaleBackgroundToWindow();

    static std::unique_ptr<Player> makePlayer2(GameMode mode);

    static std::unique_ptr<Player> makeLocalOrRemotePlayer(PlayerSide side, PlayerSide localSide, NetworkConnection& connection);

    void buildMiniMenuButton();

    void openMiniMenu();

    void handle(const sf::Event::MouseButtonPressed& event);
    void handle(const sf::Event::KeyPressed& event);
    void handle(const sf::Event::MouseMoved& event);
    // Catch-all for event types this state doesn't care about.
    void handle(const auto& event) {}

    sf::RenderWindow& m_window;
    sf::Sprite m_background;

    GameMode m_mode; // remembered only for Restart

    // Declared before m_turnManager: its constructor binds references to these.
    Board m_board;
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    TurnManager m_turnManager;

    // PlayerVsRemote only - null for Friend/AI. m_remotePlayer is a
    // non-owning pointer at whichever of m_player1/m_player2 is the RemotePlayer.
    std::unique_ptr<NetworkConnection> m_connection;
    RemotePlayer* m_remotePlayer = nullptr;

    BottomPanel m_bottomPanel;
    Tooltip m_tooltip;

    // Hand-selected monster awaiting placement. UI/gameplay-input state,
    // not board state.
    //Monster* m_selectedFromHand = nullptr;

    Card* m_selectedFromHand = nullptr;   // hand card selected, not yet placed
    Card* m_pendingSpecialCard = nullptr; // played card whose special is awaiting a target

    //BoardEntity* m_selectedEntity = nullptr;
	BoardEntity* m_selectedEntity = nullptr; // non-owning; owned by whichever Player has the entity

    std::optional<Button> m_miniMenuButton;

    void handleSpecialAbilityClick(Card* card);
    void handleSpawnAttempt(const sf::Vector2f& pos, Player& current);
    void handleSpecialTargetClick(const sf::Vector2f& pos);
    void handleBoardClick(const sf::Vector2f& pos, const Player& current);
    void clearPendingSpecial();
};
