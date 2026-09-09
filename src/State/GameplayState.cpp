#include "State/GameplayState.h"
#include "State/MiniMenuState.h"
#include "State/GameOverState.h"
#include "AIPlayer.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "Card.h"
#include <iostream>
#include "Constants.h"

namespace
{
    constexpr unsigned int MINI_MENU_BUTTON_WIDTH = 90;
    const sf::Vector2i MINI_MENU_BUTTON_POSITION = { 170, 10 };
}

std::unique_ptr<Player> GameplayState::makePlayer2(GameMode mode)
{
    if (mode == GameMode::PlayerVsAI)
        return std::make_unique<AIPlayer>(PlayerSide::Right);

    return std::make_unique<Player>(PlayerSide::Right);
}

GameplayState::GameplayState(sf::RenderWindow& window, GameMode mode)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("game_bg"))
    , m_mode(mode)
    , m_board()
    , m_player1(std::make_unique<Player>(PlayerSide::Left))
    , m_player2(makePlayer2(mode))
    , m_turnManager(*m_player1, *m_player2, m_board)
    , m_endTurnHintText(AssetsManager::getInstance().getFont("Lilita"))
{
    scaleBackgroundToWindow();
    m_board.initPlayerHearts(m_player1->getHeart(), m_player2->getHeart());
    buildMiniMenuButton();

    m_turnManager.setOnPlayerSwitched([this]() { clearSelectionState(); });

    m_endTurnHintText.setString("PRESS SPACE TO END TURN");
    m_endTurnHintText.setCharacterSize(13); // גודל קטן
    m_endTurnHintText.setFillColor(sf::Color(200, 200, 200, 180)); // צבע אפור-לבן עדין מעט שקוף

    // מיקום במרכז X ובחלק התחתון ביותר של המסך (12 פיקסלים מהקצה)
    float centerX = m_window.getSize().x / 2.0f;
    float bottomY = m_window.getSize().y - 12.0f;
    m_endTurnHintText.setPosition({ centerX, bottomY });
}

void GameplayState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}

void GameplayState::buildMiniMenuButton()
{
    auto& am = AssetsManager::getInstance();
    const sf::Texture& texture = am.getTexture("GoToMiniMenuButton");

    auto textureSize = texture.getSize();
    float scale = static_cast<float>(MINI_MENU_BUTTON_WIDTH) / static_cast<float>(textureSize.x);
    int scaledHeight = static_cast<int>(textureSize.y * scale);

    sf::IntRect rect(MINI_MENU_BUTTON_POSITION, { static_cast<int>(MINI_MENU_BUTTON_WIDTH), scaledHeight });
    m_miniMenuButton.emplace(rect, texture, [this]() { openMiniMenu(); }, sf::Vector2f{ scale, scale });
}

void GameplayState::openMiniMenu()
{
    pushState(std::make_unique<MiniMenuState>(m_window,
        [this] { transitionTo(); },
        [this] { transitionTo(std::make_unique<GameplayState>(m_window, m_mode)); }));
}

void GameplayState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);

    Player& current = m_turnManager.getCurrentPlayer();
    m_board.draw(window, current.getSide());

    drawButtomPanel(window);
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
}

void GameplayState::drawButtomPanel(sf::RenderWindow& window) const
{
	//sf::RectangleShape bottomPanel;
	//bottomPanel.setSize({ static_cast<float>(Config::WINDOW_WIDTH), 100.f });
	//bottomPanel.setPosition({ 0.f, static_cast<float>(Config::WINDOW_HEIGHT) - 100.f });
	//bottomPanel.setFillColor(sf::Color(30, 30, 50, 200)); // Darker color with some transparency
	//m_window.draw(bottomPanel);
    sf::RectangleShape bottomPanel({ static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT });
    bottomPanel.setPosition({ 0.f, Config::BOTTOM_PANEL_Y });
    bottomPanel.setFillColor(sf::Color(40, 40, 40));
    window.draw(bottomPanel);
    m_window.draw(m_endTurnHintText); //אם יש דרך יותר טובה...
}

void GameplayState::update(sf::Time deltaTime)
{
    m_board.update(deltaTime.asSeconds());
    m_turnManager.update();

    // A Special that doesn't need a target click to commit (Barzilla's
    // Empowered Attack) can be committed by something GameplayState never
    // directly calls - a normal attack resolving at animation impact, deep
    // inside m_board.update() above. There's no synchronous call site to
    // clear m_pendingSpecialCard from in that case, so this reacts instead:
    // once the pending monster's cooldown is no longer 0, its Special must
    // have committed elsewhere, so it's no longer "pending a decision" and
    // the highlight should stop. Never engages for a targeted Special
    // (specialAbilityNeedsTarget() true) - those are still only ever
    // cleared synchronously, by handleSpecialTargetClick, exactly as before.
    if (m_pendingSpecialCard)
        //למה פה??????
    {
        Monster* pendingMonster = m_pendingSpecialCard->getLinkedMonster();
        if (!pendingMonster || (!pendingMonster->specialAbilityNeedsTarget() && !pendingMonster->isSpecialReady()))
            m_pendingSpecialCard = nullptr;
    }

    if (m_player1->isDead() || m_player2->isDead())
    {
        PlayerSide winner = (m_player1->isDead()) ? PlayerSide::Right : PlayerSide::Left;

        // ������� �� m_mode �-m_window ���� ��� ����� this
        GameMode currentMode = m_mode;
        sf::RenderWindow& window = m_window;

        transitionTo(std::make_unique<GameOverState>(
            m_window,
            winner,
            [&window, currentMode]() {
                return std::make_unique<GameplayState>(window, currentMode);
            }
        ));
    }
}

void GameplayState::handleEvent(const sf::Event& event)
{
    if (m_miniMenuButton)
        m_miniMenuButton->handleEvent(event);

    event.visit([this](const auto& e) { handle(e); });
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
    Monster* monster = current.playCard(m_selectedFromHand);
    if (!monster)
        return;

    // 4. מזמנים אותה ישירות על המשבצת
    if (m_board.spawnEntityOnTile(monster, tile))
    {
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

    Monster* monster = card->getLinkedMonster();

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

    Monster* monster = m_pendingSpecialCard->getLinkedMonster();
    if (!monster) { clearPendingSpecial(); m_board.clearHighlights(); return; }

    // Reuses the same tile lookup Board already uses for board clicks -
    // no parallel targeting system. A click that doesn't land on a valid
    // target simply does nothing and stays in targeting mode, exactly like
    // an invalid spawn-position click already behaves in handleSpawnAttempt.
    const Tile* targetTile = m_board.getTileAtScreenPosition(pos);
    BoardEntity* candidate = targetTile ? targetTile->getEntity() : nullptr;

    if (candidate && monster->isValidSpecialTarget(*candidate))
    {
        if (monster->useSpecialAbility(m_board, candidate))
        {
            m_pendingSpecialCard = nullptr; // committed successfully - nothing left to cancel
            m_board.clearHighlights();
        }
    }
}

void GameplayState::handleBoardClick(const sf::Vector2f& pos, Player& current)
{
    const Tile* clickedTile = m_board.getTileAtScreenPosition(pos);
    if (!clickedTile) return;

    if (m_selectedEntity)
    {
        // אם לחצנו על משבצת חוקית (מוארת) - Board כבר תדע אם לזוז או לתקוף
        if (clickedTile->isHighlighted())
            m_board.performAction(m_selectedEntity, clickedTile);

        // ניקוי וביטול בחירה
        m_selectedEntity = nullptr;
        m_board.clearHighlights();
    }
    else if (BoardEntity* entity = clickedTile->getEntity())
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
        m_pendingSpecialCard->getLinkedMonster()->cancelSpecialAbility();
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

    if (pos.y > Config::BOTTOM_PANEL_Y)
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