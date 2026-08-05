#include "State/GameModeState.h"
#include "State/GameplayState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"

GameModeState::GameModeState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("menu_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, 310.f }, 255, 18.f)
{
    scaleBackgroundToWindow();
    buildMenu();
}

void GameModeState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}

void GameModeState::buildMenu()
{
    auto& am = AssetsManager::getInstance();

    m_menu.addButton(am.getTexture("FriendButton"), [this]() { onFriendClicked(); });
    m_menu.addButton(am.getTexture("AiButton"), [this]() { onAiClicked(); });
    m_menu.addButton(am.getTexture("BackButton"), [this]() { onBackClicked(); });
}

void GameModeState::onFriendClicked()
{
    transitionTo(std::make_unique<GameplayState>(m_window, GameMode::PlayerVsPlayer));
}

void GameModeState::onAiClicked()
{
    transitionTo(std::make_unique<GameplayState>(m_window, GameMode::PlayerVsAI));
}

void GameModeState::onBackClicked()
{
    transitionTo(); // pop back to whichever state pushed this one
}

void GameModeState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    m_menu.draw(window);
}

void GameModeState::update(sf::Time deltaTime)
{
    // Nothing to animate on the game-mode screen.
}

void GameModeState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);
}
