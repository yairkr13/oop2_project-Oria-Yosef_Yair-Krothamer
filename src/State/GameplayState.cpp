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

    // Anchored to the top-left corner, not the window's center - same
    // reasoning InstructionsState's own left-anchored nav button margin
    // uses: a corner-relative position stays correct regardless of
    // Config::WINDOW_WIDTH/HEIGHT, unlike a center-relative one, so it
    // doesn't need a Config-derived offset the way those do.
    const sf::Vector2i MINI_MENU_BUTTON_POSITION = { 170, 10 };

    // Picks "game_bg_1" or "game_bg_2" with equal odds - a fresh
    // std::mt19937 each call (not a shared static one, unlike Board::rng())
    // since this only ever runs once per GameplayState, at construction -
    // no reason for it to share state across games the way Board's own
    // board-generation randomness does.
    const char* randomGameBackgroundKey()
    {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 2);
        return dist(gen) == 1 ? "game_bg_1" : "game_bg_2";
    }
}

std::unique_ptr<Player> GameplayState::makePlayer2(GameMode mode)
{
    if (mode == GameMode::PlayerVsAI)
        return std::make_unique<AIPlayer>(PlayerSide::Right);

    return std::make_unique<Player>(PlayerSide::Right);
}

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
    // Known directly from which side was built as which above - not a cast
    // guessing at m_player1/m_player2's actual concrete type.
    m_remotePlayer = (localSide == PlayerSide::Left)
        ? static_cast<RemotePlayer*>(m_player2.get())
        : static_cast<RemotePlayer*>(m_player1.get());

    scaleBackgroundToWindow();
    m_board.initPlayerHearts(m_player1->getHeart(), m_player2->getHeart(), /*useFixedSpecialTiles=*/true);
    buildMiniMenuButton();

    m_turnManager.setOnPlayerSwitched([this]() {
        clearSelectionState();

        // We just switched TO the remote player - meaning the local
        // human's own turn just ended - so send everything recorded
        // during it now, exactly once per turn.
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
    // Card already knows how to render its own "selected" border
    // (see Card::draw's isSelected parameter) - we just need to feed it
    // whichever Card the player currently has chosen. A not-yet-placed hand
    // card (m_selectedFromHand) and a played card whose targeted Special is
    // pending a target (m_pendingSpecialCard) are mutually exclusive by
    // construction (selecting either one always clears the other), so
    // there's always at most one to show here.
    Card* visuallySelectedCard = m_selectedFromHand ? m_selectedFromHand : m_pendingSpecialCard;
    current.draw(window, &current == m_player2.get(), visuallySelectedCard); //למה הוא מעביר את זה לשחקן. השחקן צריך לדעת את זה בעצמו????

    //m_player1->drawKeys(window, false);
    //m_player2->drawKeys(window, true);

    if (m_miniMenuButton)
        m_miniMenuButton->draw(window);

    m_tooltip.draw(window);
}

void GameplayState::update(sf::Time deltaTime)
{
    // Services the connection every frame regardless of whose turn it
    // locally is - sends anything still queued, and buffers anything
    // arriving, so a message is never sitting unread in the OS socket
    // buffer just because it happened to arrive mid-animation.
    if (m_remotePlayer)
        m_remotePlayer->pollIncoming();

    m_board.update(deltaTime.asSeconds());

    // GameplayState is the only thing that ever holds a raw Card* into
    // either player's hand (m_selectedFromHand/m_pendingSpecialCard) - drop
    // both here, first, whenever they point at a Card whose monster just
    // died (Card::isGone()), so removeDeadMonsters() below can safely erase
    // that Card outright without leaving either pointer dangling.
    if (m_selectedFromHand && m_selectedFromHand->isGone())
        m_selectedFromHand = nullptr;
    if (m_pendingSpecialCard && m_pendingSpecialCard->isGone())
    {
        clearPendingSpecial();
        m_board.clearHighlights();
    }

    // Erases each player's Cards whose monster just became isGone() - same
    // per-frame granularity Board::update() itself already cleans up
    // entities at, so a Card never lingers (visible or clickable) past the
    // very frame its monster actually died (see
    // Monster::canUseSpecialAbilityNow()/Player::getCardTooltipAt, both of
    // which depend on a dead monster's Card being gone promptly).
    m_player1->removeDeadMonsters();
    m_player2->removeDeadMonsters();

    m_turnManager.update();

    // Old safety net, kept as a comment - it existed for a Special that
    // doesn't need a target click to commit (Barzilla's old self-buff
    // Empowered Attack, committed later by a normal attack resolving deep
    // inside m_board.update() above, with no synchronous call site to clear
    // m_pendingSpecialCard from). Now that every monster's Special commits
    // synchronously on target selection (specialAbilityNeedsTarget() is true
    // for all five - see Monster.h), m_pendingSpecialCard is always cleared
    // directly by handleSpecialTargetClick, and this reactive check would
    // never trigger (its guard condition can no longer be true for any
    // monster):
    //
    // if (m_pendingSpecialCard)
    // {
    //     Monster* pendingMonster = m_pendingSpecialCard->getLinkedMonster();
    //     if (!pendingMonster || (!pendingMonster->specialAbilityNeedsTarget() && !pendingMonster->isSpecialReady()))
    //         m_pendingSpecialCard = nullptr;
    // }

    if (m_player1->isDead() || m_player2->isDead())
    {
        PlayerSide winner = (m_player1->isDead()) ? PlayerSide::Right : PlayerSide::Left;

        // The killing action usually lands mid-turn, before the local human
        // ever ends their own turn - so without this, it (and anything else
        // recorded this turn) would sit unsent in m_recordedActions forever,
        // since sendRecordedActions() otherwise only fires from
        // setOnPlayerSwitched, which this transition preempts. Flushing here
        // guarantees the peer receives it, replays it, and its own Board
        // reaches this same dead state - so its own isDead() check above
        // fires independently right after, with no separate "you lost"
        // message needed at all.
        if (m_remotePlayer)
            m_remotePlayer->sendRecordedActions();

        // ������� �� m_mode �-m_window ���� ��� ����� this
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

    if (mousePos.y > static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT)
    {
        // Player מחזיר רק טקסט - הקפסולציה נשמרה לחלוטין!
        std::string tooltipText = current.getCardTooltipAt(mousePos);

        if (!tooltipText.empty())
        {
            m_tooltip.show(tooltipText, mousePos);
            return;
        }
    }

    m_tooltip.hide();
}

//void GameplayState::handleSpawnAttempt(const sf::Vector2f& pos, Player& current)
//{
//    // ������ ������ *����* playCard - Board �� ���� ���� �� Card, �� �� geometry
//    if (!m_selectedFromHand)
//        return;
//    if (!m_board.isSpawnPositionValid(pos))
//        return;
//
//    Monster* monster = current.playCard(m_selectedFromHand); // Player ���� �cost+ownership
//    if (!monster)
//        return; // �� ���� ����� �� ������ ����� ����, ��� ������ �� �����
//    
//    if (m_board.trySpawnMonster(pos, monster)) // Board ���� Monster ����, �� Card
//    {
//        m_selectedFromHand = nullptr; // clearHighlights ��� ���� ���� trySpawnMonster ������
//        m_board.clearHighlights();
//    }
//}
void GameplayState::handleSpawnAttempt(const sf::Vector2f& pos, Player& current)
{
    if (!m_selectedFromHand)
        return;

    // 1. משיגים את המשבצת שעליה לחצו
    const Tile* tile = m_board.getTileAtScreenPosition(pos);

    // 2. בודקים שהיא קיימת ומודגשת (כלומר חוקית לזימון)
    if (!tile || !tile->isHighlighted())
        return;

    // 3. משלמים את העלות ויוצרים את המפלצת
    int cardIndex = current.indexOfCard(m_selectedFromHand); // before playCard - still in hand right now
    Monster* monster = current.playCard(m_selectedFromHand);
    if (!monster)
        return;

    // 4. מזמנים אותה ישירות על המשבצת
    if (m_board.spawnEntityOnTile(monster, tile))
    {
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

void GameplayState::handleSpecialAbilityClick(Card* card)
{
    if (!card->isPlayed())
        return;

    // Toggle: clicking the Card that's already pending cancels the
    // selection instead of re-entering it - see clearPendingSpecial() for
    // why this is always safe to call even when nothing was actually armed.
    if (card == m_pendingSpecialCard)
    {
        clearPendingSpecial();
        m_board.clearHighlights();
        return;
    }

    // getMutableLinkedMonster() (not getLinkedMonster()) - useSpecialAbility below actually mutates monster.
    Monster* monster = card->getMutableLinkedMonster();

    // Purely a readiness check - does not commit/reset anything. Asks the
    // same single predicate useSpecialAbility() re-checks internally at
    // commit time, rather than reconstructing the rule here too.
    if (!monster || !monster->canUseSpecialAbilityNow())
        return;

    // Switching selection away from whatever was pending before (if
    // anything) must not leave it silently armed in the background.
    clearPendingSpecial();

    if (monster->specialAbilityNeedsTarget())
    {
        // Enter target-selection mode only - the Special is not activated
        // and the cooldown is not touched until a valid target is actually
        // clicked (see handleSpecialTargetClick). GameplayState never asks
        // which monster this is, only whether it needs a target at all.
        m_pendingSpecialCard = card;
        m_selectedFromHand = nullptr;
        m_board.clearHighlights();
        m_board.highlightValidSpecialTargets(monster);
    }
    else if (monster->useSpecialAbility(m_board))
    {
        if (m_remotePlayer)
        {
            GameAction action;
            action.type = GameAction::Type::Special;
            action.cardIndex = m_turnManager.getCurrentPlayer().indexOfCard(card);
            action.hasTarget = false;
            m_remotePlayer->recordLocalAction(action);
        }

        // Committed immediately (the common case) unless this monster's
        // Special is armed now and actually used at a later, separate event
        // (Barzilla) - specialAbilityCommitsOnSelect() already told
        // useSpecialAbility() whether to do that bookkeeping yet or not.
        // Either way, the Card stays visibly selected/cancelable until that
        // event happens - update() clears it reactively once it does.
        if (!monster->specialAbilityCommitsOnSelect())
        {
            m_pendingSpecialCard = card;
            m_selectedFromHand = nullptr;
        }

        // Immediately reflect whatever the Special just changed about this
        // monster's own move/attack options (e.g. Barzilla's Empowered
        // Attack extending his attack range) - equivalent to the player
        // clicking the monster on the board right after activating the
        // Special, without requiring that extra click. Board::selectEntity
        // is the exact same select-and-highlight flow a direct board click
        // already uses, so nothing about range/highlighting is re-derived
        // here. Only when there's still an action left to use it with -
        // otherwise there's nothing this monster could still do this turn.
        if (monster->getActionsLeft() > 0)
        {
            // A pending hand-spawn selection would otherwise be left
            // dangling: selectEntity() below repaints the whole board's
            // highlights, which would silently wipe the spawn-tile
            // highlighting out from under it without this.
            m_selectedFromHand = nullptr;

            // Equivalent to the player clicking this monster on the board
            // right after activating the Special - remember it as the
            // current board selection ourselves (Board no longer tracks
            // this), so the next board click acts on it exactly like a
            // direct click-then-click would.
            if (m_board.selectEntity(monster, monster->getSide()))
                m_selectedEntity = monster;
        }
    }
}

void GameplayState::handleSpecialTargetClick(const sf::Vector2f& pos)
{
    if (!m_pendingSpecialCard) return;

    // getMutableLinkedMonster() (not getLinkedMonster()) - useSpecialAbility below actually mutates monster.
    Monster* monster = m_pendingSpecialCard->getMutableLinkedMonster();
    if (!monster) { clearPendingSpecial(); m_board.clearHighlights(); return; }

    // Reuses the same tile lookup Board already uses for board clicks -
    // no parallel targeting system. A click that doesn't land on a valid
    // target simply does nothing and stays in targeting mode, exactly like
    // an invalid spawn-position click already behaves in handleSpawnAttempt.
    const Tile* targetTile = m_board.getTileAtScreenPosition(pos);
    // getMutableEntity() (not getEntity()) - useSpecialAbility below actually mutates candidate.
    BoardEntity* candidate = targetTile ? targetTile->getMutableEntity() : nullptr;

    if (candidate && monster->isValidSpecialTarget(*candidate))
    {
        if (monster->useSpecialAbility(m_board, candidate))
        {
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

void GameplayState::handleBoardClick(const sf::Vector2f& pos, const Player& current)
{
    const Tile* clickedTile = m_board.getTileAtScreenPosition(pos);
    if (!clickedTile) return;

    if (m_selectedEntity)
    {
        // אם לחצנו על משבצת חוקית (מוארת) - Board כבר תדע אם לזוז או לתקוף
        if (clickedTile->isHighlighted())
        {
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

        // ניקוי וביטול בחירה
        m_selectedEntity = nullptr;
        m_board.clearHighlights();
    }
    else if (BoardEntity* entity = clickedTile->getMutableEntity()) // m_selectedEntity below is later handed to Board::performAction, which mutates it
    {
        // השורה שלך! בדיקה פולימורפית נקייה - ללא asMonster() וללא Casting
        /*if (entity->canBeSelectedBy(current.getSide()))
        {*/
            /*m_selectedEntity = entity;
            m_board.highlightNeighbors(m_selectedEntity);*/
        if (m_board.selectEntity(entity, current.getSide()))
            m_selectedEntity = entity;
       /* }*/
    }
    //else if (BoardEntity* entity = clickedTile->getEntity())
    //{
    //    // פולימורפיזם מלא: לא צריך לדעת שזו מפלצת, רק שזו ישות שניתן לבחור כרגע
    //    if (m_board.selectEntity(entity, current.getSide()))
    //        m_selectedEntity = entity;
    //}
}

void GameplayState::clearPendingSpecial()
{
    if (m_pendingSpecialCard)
        m_pendingSpecialCard->getMutableLinkedMonster()->cancelSpecialAbility();
    m_pendingSpecialCard = nullptr;
}

//void GameplayState::highlightValidSpecialTargets(Monster& caster)
//{
//    std::vector<const Tile*> validTargets;
//    for (const Tile* tile : m_board.getOccupiedTiles())
//    {
//        const BoardEntity* candidate = tile->getEntity();
//        if (candidate && caster.isValidSpecialTarget(*candidate))
//            validTargets.push_back(tile);
//    }
//
//    // Board owns painting its own Tiles - this only ever decides *which*
//    // tiles qualify and *what color*, both of which stay entirely up to
//    // `caster` (see Monster::isValidSpecialTarget/getSpecialTargetHighlightColor).
//    m_board.highlightTiles(validTargets, caster.getSpecialTargetHighlightColor());
//}

void GameplayState::handle(const sf::Event::MouseButtonPressed& event)
{
    if (event.button != sf::Mouse::Button::Left)
        return;

    if (!m_turnManager.canAcceptInput())
        return;

    sf::Vector2f pos = m_window.mapPixelToCoords(event.position);

    Player& current = m_turnManager.getCurrentPlayer();

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
                //למה הוא אחראי לזה??????
                m_board.highlightSpawnTiles(current.getSide());//למה מעבירים מפלצת?????? מיותר קצת
            }
        }
        /*auto clickedMonster = current.handleHandClick(pos, isPlayer2);
        if (clickedMonster)
        {
            if (m_selectedFromHand == clickedMonster)
            {
                m_selectedFromHand = nullptr;
                m_board.clearHighlights();
            }
            else
            {
                m_selectedFromHand = clickedMonster;
                m_board.highlightSpawnTiles(current.getSide());
            }
        }*/
    }
    else
    {
        // A pending Special only intercepts board clicks as target-selection
        // if it actually needs a target - an armed-but-not-target-based
        // Special (Barzilla) leaves board clicks to behave completely
        // normally (move/attack), since arming it isn't a targeting
        // decision and the board is exactly where its commit event (a
        // normal attack) has to be able to happen.
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