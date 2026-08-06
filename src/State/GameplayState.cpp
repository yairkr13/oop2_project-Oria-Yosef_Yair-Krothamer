#include "State/GameplayState.h"
#include "State/MiniMenuState.h"
#include "State/GameOverState.h"
#include "AIPlayer.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
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
{
    scaleBackgroundToWindow();
    m_board.initPlayerHearts(m_player1->getHeart(), m_player2->getHeart());
    buildMiniMenuButton();
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
    current.draw(window, &current == m_player2.get(), m_selectedFromHand);

    m_player1->drawKeys(window, false);
    m_player2->drawKeys(window, true);

    if (m_miniMenuButton)
        m_miniMenuButton->draw(window);
}

void GameplayState::update(sf::Time deltaTime)
{
    m_board.update(deltaTime.asSeconds());
    m_turnManager.update();

    if (m_player1->isDead() || m_player2->isDead())
    {
        PlayerSide winner = (m_player1->isDead()) ? PlayerSide::Right : PlayerSide::Left;

        // מעבירים את m_mode ו-m_window בלבד ללא לכידת this
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

        auto clickedMonster = current.handleHandClick(pos, isPlayer2);
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
        }
    }
    else
    {
        if (m_selectedFromHand)
        {
            bool success = m_board.trySpawnMonster(pos, m_selectedFromHand);
            if (success)
            {
                current.reduceKeys(m_selectedFromHand->getCost());
                m_board.clearHighlights();
                m_selectedFromHand = nullptr;
            }
        }
        else
        {
            m_board.handleClick(pos, current.getSide());
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
        m_turnManager.requestEndTurn();
        m_selectedFromHand = nullptr;
    }
}
