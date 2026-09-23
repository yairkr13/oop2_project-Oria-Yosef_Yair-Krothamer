#include "State/GameplayState.h"
#include "State/MiniMenuState.h"
#include "State/GameOverState.h"
#include "AIPlayer.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "Card.h"
#include <iostream>
#include "Constants.h"
#include <random>

namespace
{
    constexpr unsigned int MINI_MENU_BUTTON_WIDTH = 90;

    // Anchored to the top-left corner, so it stays correct regardless of window size.
    const sf::Vector2i MINI_MENU_BUTTON_POSITION = { 170, 10 };

    // Picks "game_bg_1" or "game_bg_2" with equal odds; a fresh RNG per
    // call, since this only ever runs once per GameplayState.
    const char* randomGameBackgroundKey()
    {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 2);
        return dist(gen) == 1 ? "game_bg_1" : "game_bg_2";
    }
}

// AIPlayer for PlayerVsAI, otherwise a plain Player - a helper since the two
// unique_ptr types don't share a common type in an inline ternary.
std::unique_ptr<Player> GameplayState::makePlayer2(GameMode mode)
{
    if (mode == GameMode::PlayerVsAI)
        return std::make_unique<AIPlayer>(PlayerSide::Right);

    return std::make_unique<Player>(PlayerSide::Right);
}

// A local Player if `side` matches localSide, else a RemotePlayer wired to `connection`.
std::unique_ptr<Player> GameplayState::makeLocalOrRemotePlayer(PlayerSide side, PlayerSide localSide, NetworkConnection& connection)
{
    if (side == localSide)
        return std::make_unique<Player>(side);

    return std::make_unique<RemotePlayer>(side, connection);
}

GameplayState::GameplayState(sf::RenderWindow& window, GameMode mode)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture(randomGameBackgroundKey()))
    , m_mode(mode)
    , m_board(BoardGenerator::standardLayout())
    , m_player1(std::make_unique<Player>(PlayerSide::Left))
    , m_player2(makePlayer2(mode))
    , m_turnManager(*m_player1, *m_player2, m_board)
    , m_bottomPanel(AssetsManager::getInstance().getFont("Lilita"))
    , m_tooltip(AssetsManager::getInstance().getFont("Lilita"))
{
    scaleBackgroundToWindow();
    m_board.initPlayerHearts(m_player1->getHeart(), m_player2->getHeart());
    buildMiniMenuButton();

    m_turnManager.setOnPlayerSwitched([this]() { clearSelectionState(); });
}

// PlayerVsRemote only - `connection` must already be connected (see
// NetworkLobbyState). `localSide` is which side this computer's human
// plays (host is always Left, joiner always Right).
GameplayState::GameplayState(sf::RenderWindow& window, std::unique_ptr<NetworkConnection> connection, PlayerSide localSide)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture(randomGameBackgroundKey()))
    , m_mode(GameMode::PlayerVsRemote)
    , m_board(BoardGenerator::standardLayout())
    , m_player1(makeLocalOrRemotePlayer(PlayerSide::Left, localSide, *connection))
    , m_player2(makeLocalOrRemotePlayer(PlayerSide::Right, localSide, *connection))
    , m_turnManager(*m_player1, *m_player2, m_board)
    , m_bottomPanel(AssetsManager::getInstance().getFont("Lilita"))
    , m_tooltip(AssetsManager::getInstance().getFont("Lilita"))
    , m_connection(std::move(connection))
{
    // Known directly from which side was built as which above, not a cast.
    m_remotePlayer = (localSide == PlayerSide::Left)
        ? static_cast<RemotePlayer*>(m_player2.get())
        : static_cast<RemotePlayer*>(m_player1.get());

    scaleBackgroundToWindow();
    m_board.initPlayerHearts(m_player1->getHeart(), m_player2->getHeart(), /*useFixedSpecialTiles=*/true);
    buildMiniMenuButton();

    m_turnManager.setOnPlayerSwitched([this]() {
        clearSelectionState();

        // Switched to the remote player - the local human's turn just ended, so flush it now.
        if (m_remotePlayer && &m_turnManager.getCurrentPlayer() == static_cast<Player*>(m_remotePlayer))
            m_remotePlayer->sendRecordedActions();
    });
}

void GameplayState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}

void GameplayState::buildMiniMenuButton()
{
    const sf::Texture& texture = AssetsManager::getInstance().getTexture("GoToMiniMenuButton");
    m_miniMenuButton = Button::fromTextureWidth(MINI_MENU_BUTTON_POSITION, texture, MINI_MENU_BUTTON_WIDTH,
        [this]() { openMiniMenu(); });
}

// Pushes MiniMenuState, wiring Restart/Exit to finish this paused state -
// shared by Escape and the on-board button so they can't drift apart.
void GameplayState::openMiniMenu()
{
    // No restart mid-match over the network - see MiniMenuState.
    std::function<void()> onRestartGame = nullptr;
    if (m_mode != GameMode::PlayerVsRemote)
        onRestartGame = [this] { transitionTo(std::make_unique<GameplayState>(m_window, m_mode)); };

    pushState(std::make_unique<MiniMenuState>(m_window,
        [this] { transitionTo(); },
        std::move(onRestartGame)));
}

void GameplayState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);

    Player& current = m_turnManager.getCurrentPlayer();
    m_board.draw(window, current.getSide());

    m_bottomPanel.draw(window);
    // The not-yet-placed hand card and a pending-target special card are
    // mutually exclusive by construction - at most one to show here.
    Card* visuallySelectedCard = m_selectedFromHand ? m_selectedFromHand : m_pendingSpecialCard;
    current.draw(window, &current == m_player2.get(), visuallySelectedCard);

    if (m_miniMenuButton)
        m_miniMenuButton->draw(window);

    m_tooltip.draw(window);
}

void GameplayState::update(sf::Time deltaTime)
{
    // Services the connection every frame regardless of whose turn it locally is.
    if (m_remotePlayer)
        m_remotePlayer->pollIncoming();

    m_board.update(deltaTime.asSeconds());

    // Drop any raw Card* pointer into a hand whose monster just died, before
    // removeDeadMonsters() below erases that Card and leaves it dangling.
    if (m_selectedFromHand && m_selectedFromHand->isGone())
        m_selectedFromHand = nullptr;
    if (m_pendingSpecialCard && m_pendingSpecialCard->isGone())
    {
        clearPendingSpecial();
        m_board.clearHighlights();
    }

    // Erases each player's Cards whose monster just died, same per-frame
    // granularity Board::update() cleans up entities at.
    m_player1->removeDeadMonsters();
    m_player2->removeDeadMonsters();

    m_turnManager.update();

    // Game ends the instant either side's Heart dies - checked every frame
    // so the transition fires as soon as the killing action resolves.
    if (m_player1->isDead() || m_player2->isDead())
    {
        PlayerSide winner = (m_player1->isDead()) ? PlayerSide::Right : PlayerSide::Left;

        // The kill usually lands mid-turn, before setOnPlayerSwitched would
        // otherwise flush it - flush explicitly so the peer reaches the same
        // dead state and its own isDead() check fires independently.
        if (m_remotePlayer)
            m_remotePlayer->sendRecordedActions();

        // Captured by value, not `this` - this GameplayState is about to transition away.
        GameMode currentMode = m_mode;
        sf::RenderWindow& window = m_window;

        // No "Play Again" over the network - see GameOverState.
        std::function<std::unique_ptr<State>()> createNextState = nullptr;
        if (currentMode != GameMode::PlayerVsRemote)
        {
            createNextState = [&window, currentMode]() {
                return std::make_unique<GameplayState>(window, currentMode);
            };
        }

        transitionTo(std::make_unique<GameOverState>(
            m_window,
            winner,
            std::move(createNextState)
        ));
    }
}

void GameplayState::handleEvent(const sf::Event& event)
{
    if (m_miniMenuButton)
        m_miniMenuButton->handleEvent(event);

    event.visit([this](const auto& e) { handle(e); });
}

void GameplayState::handle(const sf::Event::MouseMoved& event)
{
    sf::Vector2f mousePos = m_window.mapPixelToCoords(event.position);

    Player& current = m_turnManager.getCurrentPlayer();

    // Only show hand-card tooltips while hovering the bottom panel.
    if (mousePos.y > static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT)
    {
        std::string tooltipText = current.getCardTooltipAt(mousePos);

        if (!tooltipText.empty())
        {
            m_tooltip.show(tooltipText, mousePos);
            return;
        }
    }

    m_tooltip.hide();
}

// Handles a tile click while a hand card is selected for placement -
// validates the target before paying cost and spawning.
void GameplayState::handleSpawnAttempt(const sf::Vector2f& pos, Player& current)
{
    if (!m_selectedFromHand)
        return;

    const Tile* tile = m_board.getTileAtScreenPosition(pos);
    if (!tile || !tile->isHighlighted())
        return;

    int cardIndex = current.indexOfCard(m_selectedFromHand); // before playCard - still in hand right now
    Monster* monster = current.playCard(m_selectedFromHand);
    if (!monster)
        return;

    if (m_board.spawnEntityOnTile(monster, tile))
    {
        // Recorded so the peer replays this action on their own board.
        if (m_remotePlayer)
        {
            GameAction action;
            action.type = GameAction::Type::Spawn;
            action.cardIndex = cardIndex;
            action.hasTarget = true;
            action.targetQ = tile->getQ();
            action.targetRow = tile->getRow();
            m_remotePlayer->recordLocalAction(action);
        }

        m_selectedFromHand = nullptr;
        m_board.clearHighlights();
    }
}

// Handles a click on an already-played card (showing "READY"/"CD: X") -
// arms its special ability's target-selection mode.
void GameplayState::handleSpecialAbilityClick(Card* card)
{
    if (!card->isPlayed())
        return;

    // Clicking the already-pending card cancels it instead of re-arming.
    if (card == m_pendingSpecialCard)
    {
        clearPendingSpecial();
        m_board.clearHighlights();
        return;
    }

    // getMutableLinkedMonster() (not getLinkedMonster()) - useSpecialAbility below actually mutates monster.
    Monster* monster = card->getMutableLinkedMonster();

    // Readiness check only - mirrors the same predicate useSpecialAbility() re-checks at commit time.
    if (!monster || !monster->canUseSpecialAbilityNow())
        return;

    // Don't leave whatever was pending before silently armed.
    clearPendingSpecial();

    if (monster->specialAbilityNeedsTarget())
    {
        // Enter target-selection mode - not activated/cooled down until a target is actually clicked.
        m_pendingSpecialCard = card;
        m_selectedFromHand = nullptr;
        m_board.clearHighlights();
        m_board.highlightValidSpecialTargets(monster);
    }
    else if (monster->useSpecialAbility(m_board))
    {
        // Recorded so the peer replays this action on their own board.
        if (m_remotePlayer)
        {
            GameAction action;
            action.type = GameAction::Type::Special;
            action.cardIndex = m_turnManager.getCurrentPlayer().indexOfCard(card);
            action.hasTarget = false;
            m_remotePlayer->recordLocalAction(action);
        }

        // Stays selected until its own later commit event (e.g. Barzilla)
        // instead of committing here; update() clears it once that happens.
        if (!monster->specialAbilityCommitsOnSelect())
        {
            m_pendingSpecialCard = card;
            m_selectedFromHand = nullptr;
        }

        // Reflects the Special's effect on this monster's move/attack
        // options immediately, as if it were just clicked on the board.
        if (monster->getActionsLeft() > 0)
        {
            // Avoids selectEntity() below silently wiping a pending spawn-tile highlight.
            m_selectedFromHand = nullptr;

            // Remember this as the current board selection, same as a direct click would.
            if (m_board.selectEntity(monster, monster->getSide()))
                m_selectedEntity = monster;
        }
    }
}

// Handles a tile click while a special ability is awaiting its target.
void GameplayState::handleSpecialTargetClick(const sf::Vector2f& pos)
{
    if (!m_pendingSpecialCard) return;

    // getMutableLinkedMonster() (not getLinkedMonster()) - useSpecialAbility below actually mutates monster.
    Monster* monster = m_pendingSpecialCard->getMutableLinkedMonster();
    if (!monster) { clearPendingSpecial(); m_board.clearHighlights(); return; }

    // Same tile lookup as a normal board click; an invalid target just stays in targeting mode.
    const Tile* targetTile = m_board.getTileAtScreenPosition(pos);
    // getMutableEntity() (not getEntity()) - useSpecialAbility below actually mutates candidate.
    BoardEntity* candidate = targetTile ? targetTile->getMutableEntity() : nullptr;

    // Clicked tile must actually hold something, and that something must be a legal target for this monster's Special.
    if (candidate && monster->isValidSpecialTarget(*candidate))
    {
        if (monster->useSpecialAbility(m_board, candidate))
        {
            // Recorded so the peer replays this action on their own board.
            if (m_remotePlayer)
            {
                GameAction action;
                action.type = GameAction::Type::Special;
                action.cardIndex = m_turnManager.getCurrentPlayer().indexOfCard(m_pendingSpecialCard);
                action.hasTarget = true;
                action.targetQ = candidate->getQ();
                action.targetRow = candidate->getRow();
                m_remotePlayer->recordLocalAction(action);
            }

            m_pendingSpecialCard = nullptr; // committed successfully - nothing left to cancel
            m_board.clearHighlights();
        }
    }
}

// Interprets a plain board click: select an entity, then click a highlighted
// tile to move/attack with it, or click elsewhere to deselect.
void GameplayState::handleBoardClick(const sf::Vector2f& pos, const Player& current)
{
    const Tile* clickedTile = m_board.getTileAtScreenPosition(pos);
    if (!clickedTile) return;

    if (m_selectedEntity)
    {
        // Board decides move vs attack based on the highlighted tile.
        if (clickedTile->isHighlighted())
        {
            // Recorded so the peer replays this action on their own board.
            if (m_remotePlayer)
            {
                GameAction action;
                action.type = GameAction::Type::MoveOrAttack;
                action.sourceQ = m_selectedEntity->getQ();
                action.sourceRow = m_selectedEntity->getRow();
                action.hasTarget = true;
                action.targetQ = clickedTile->getQ();
                action.targetRow = clickedTile->getRow();
                m_remotePlayer->recordLocalAction(action);
            }

            m_board.performAction(m_selectedEntity, clickedTile);
        }

        m_selectedEntity = nullptr;
        m_board.clearHighlights();
    }
    else if (BoardEntity* entity = clickedTile->getMutableEntity()) // m_selectedEntity below is later handed to Board::performAction, which mutates it
    {
        if (m_board.selectEntity(entity, current.getSide()))
            m_selectedEntity = entity;
    }
}

// Un-arms whatever the pending Card's monster may have armed, before it's abandoned.
void GameplayState::clearPendingSpecial()
{
    if (m_pendingSpecialCard)
        m_pendingSpecialCard->getMutableLinkedMonster()->cancelSpecialAbility();
    m_pendingSpecialCard = nullptr;
}

void GameplayState::handle(const sf::Event::MouseButtonPressed& event)
{
    if (event.button != sf::Mouse::Button::Left)
        return;

    if (!m_turnManager.canAcceptInput())
        return;

    sf::Vector2f pos = m_window.mapPixelToCoords(event.position);

    Player& current = m_turnManager.getCurrentPlayer();

    // Click landed on the bottom hand panel, not the board - route it to hand-click handling below.
    if (pos.y > static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT)
    {
        bool isPlayer2 = (&current == m_player2.get());

        Card* clickedCard = current.handleHandClick(pos, isPlayer2);

        if (clickedCard)
        {
            if (clickedCard->isPlayed())
            {
                handleSpecialAbilityClick(clickedCard);
            }
            else if (m_selectedFromHand == clickedCard)
            {
                m_selectedFromHand = nullptr;
                m_board.clearHighlights();
            }
            else
            {
                SoundPlayer::getInstance().play("activate_card");

                m_selectedFromHand = clickedCard;
                clearPendingSpecial();
                m_board.highlightSpawnTiles(current.getSide());
            }
        }
    }
    else
    {
        // A pending special only intercepts clicks if it needs a target -
        // an armed-but-untargeted special (Barzilla) leaves clicks as normal move/attack.
        if (m_pendingSpecialCard && m_pendingSpecialCard->getLinkedMonster()->specialAbilityNeedsTarget())
        {
            handleSpecialTargetClick(pos);
        }
        else if (m_selectedFromHand)
        {
            handleSpawnAttempt(pos, current);
        }
        else
        {
            handleBoardClick(pos, current);
        }
    }
}

void GameplayState::handle(const sf::Event::KeyPressed& event)
{
    if (event.code == sf::Keyboard::Key::Escape)
    {
        openMiniMenu();
        return;
    }

    // Only end the turn on Space if it's actually this player's turn to act.
    if (event.code == sf::Keyboard::Key::Space && m_turnManager.canAcceptInput())
    {
        if (m_board.isAnimating())
            return;
        m_turnManager.requestEndTurn();
    }
}

void GameplayState::clearSelectionState()
{
    clearPendingSpecial();
    m_selectedFromHand = nullptr;
    m_selectedEntity = nullptr;
    m_board.clearHighlights();
}

MusicTrack GameplayState::desiredMusicTrack() const
{
    return MusicTrack::Gameplay;
}