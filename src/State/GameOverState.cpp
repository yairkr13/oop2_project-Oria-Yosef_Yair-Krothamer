#include "State/GameOverState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "State/MenuState.h"

GameOverState::GameOverState(sf::RenderWindow& window,const PlayerSide& winner,
    std::function<std::unique_ptr<State>()> createNextState)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("gameover_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, 260.f }, 340, 22.f)
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

    //m_menu.addButton(buttonTexture, [this]() { onResumeClicked(); }, font, "Resume");
    //m_menu.addButton(buttonTexture, [this]() { onInstructionsClicked(); }, font, "Instructions");
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

void GameOverState::onExitClicked()
{
    m_window.close();
}

void GameOverState::onMainMenuClicked()
{
    // מעבר בחזרה לתפריט הראשי
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
    // ניתן להוסיף כאן אפקטים או אנימציות לטקסט אם רוצים
}

void GameOverState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);
}

void GameOverState::setupWinnerText(const PlayerSide& winner)
{
    std::string text = (winner == PlayerSide::Left) ? "Player 1 Wins!" : "Player 2 Wins!";

    m_winnerText.setString(text);
    m_winnerText.setCharacterSize(48);
    m_winnerText.setFillColor(sf::Color::Yellow);
    m_winnerText.setStyle(sf::Text::Bold);

    sf::FloatRect textRect = m_winnerText.getLocalBounds();
    m_winnerText.setOrigin({ textRect.position.x + textRect.size.x / 2.f, textRect.position.y + textRect.size.y / 2.f });
    m_winnerText.setPosition({ static_cast<float>(m_window.getSize().x) / 2.f, static_cast<float>(m_window.getSize().y) / 4.5f });
}