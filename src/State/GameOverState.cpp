#include "State/GameOverState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "State/MenuState.h"
#include "Constants.h"

namespace
{
    // Offset from the window's own vertical center, Config-based instead of a fixed pixel value.
    constexpr float MENU_Y_OFFSET_FROM_CENTER = -100.f;

    // Same convention as MENU_Y_OFFSET_FROM_CENTER above.
    constexpr float WINNER_TEXT_Y_OFFSET_FROM_CENTER = -200.f;
}

GameOverState::GameOverState(sf::RenderWindow& window,const PlayerSide& winner,
    std::function<std::unique_ptr<State>()> createNextState)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("gameover_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + MENU_Y_OFFSET_FROM_CENTER }, 340, 22.f)
    , m_winnerText(AssetsManager::getInstance().getFont("Lilita"))
    , m_createNextState(std::move(createNextState)) 
{
    scaleBackgroundToWindow();
    setupWinnerText(winner);
    buildMenu();
}

void GameOverState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}


void GameOverState::buildMenu()
{
    auto& am = AssetsManager::getInstance();
    const sf::Texture& buttonTexture = am.getTexture("GenericButton");
    const sf::Font& font = am.getFont("Lilita");

    if (m_createNextState)
        m_menu.addButton(buttonTexture, [this]() { onRestartClicked(); }, font, "Play Again");

    m_menu.addButton(buttonTexture, [this]() { onMainMenuClicked(); }, font, "Main Menu");
}

void GameOverState::onRestartClicked()
{
    if (m_createNextState)
    {
        transitionTo(m_createNextState());
    }
}

void GameOverState::onMainMenuClicked()
{
    transitionTo(std::make_unique<MenuState>(m_window));
}

void GameOverState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    window.draw(m_winnerText);
    m_menu.draw(window);
}

void GameOverState::update(sf::Time deltaTime)
{
    // Nothing to animate on the game-over screen.
}

void GameOverState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);
}

void GameOverState::setupWinnerText(const PlayerSide& winner)
{
    std::string text = (winner == PlayerSide::Left) ? "LEFT PLAYER WINS" : "RIGHT PLAYER WINS";

    m_winnerText.setString(text);
    m_winnerText.setCharacterSize(48);
    m_winnerText.setFillColor(sf::Color::Yellow);
    m_winnerText.setStyle(sf::Text::Bold);

    sf::FloatRect textRect = m_winnerText.getLocalBounds();
    m_winnerText.setOrigin({ textRect.position.x + textRect.size.x / 2.f, textRect.position.y + textRect.size.y / 2.f });
    m_winnerText.setPosition({ static_cast<float>(m_window.getSize().x) / 2.f,
        static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + WINNER_TEXT_Y_OFFSET_FROM_CENTER });
}