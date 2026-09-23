#include "State/GameModeState.h"
#include "State/GameplayState.h"
#include "State/NetworkLobbyState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    // Offset from the window's own vertical center, Config-based instead of a fixed pixel value.
    constexpr float MENU_Y_OFFSET_FROM_CENTER = -50.f;

    // BackButton.png is much taller relative to its width than the other
    // three buttons, so it needs its own smaller width to avoid overflowing.
    constexpr unsigned int BACK_BUTTON_WIDTH = 110;
}

GameModeState::GameModeState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("menu_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + MENU_Y_OFFSET_FROM_CENTER }, 255, 18.f)
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
    m_menu.addButton(am.getTexture("RemoteButton"), [this]() { onRemoteClicked(); });
    m_menu.addButton(am.getTexture("BackButton"), [this]() { onBackClicked(); }, BACK_BUTTON_WIDTH);
}

void GameModeState::onFriendClicked()
{
    transitionTo(std::make_unique<GameplayState>(m_window, GameMode::PlayerVsPlayer));
}

void GameModeState::onAiClicked()
{
    transitionTo(std::make_unique<GameplayState>(m_window, GameMode::PlayerVsAI));
}

void GameModeState::onRemoteClicked()
{
    transitionTo(std::make_unique<NetworkLobbyState>(m_window));
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
