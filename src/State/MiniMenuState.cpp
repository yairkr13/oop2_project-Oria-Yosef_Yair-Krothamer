#include "State/MiniMenuState.h"
#include "State/InstructionsState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"

namespace
    //בכל הפרויקט יש יותר מדי const שהם עם מספרים. בא לי בהמשך לעשות את הכל לפי הגודל של החלון או הלוח!!!!!!
{
    constexpr unsigned int POPUP_WIDTH = 640;
    constexpr unsigned int MENU_BUTTON_WIDTH = 140;
    constexpr float MENU_GAP = 12.f;
    constexpr unsigned int AUDIO_BUTTON_WIDTH = 60;
    constexpr int BUTTON_GAP_Y = 12;
    const sf::Vector2i MUSIC_BUTTON_POSITION = { 497, 228 };
    sf::Vector2i soundButtonPosition()
    {
        return {
            MUSIC_BUTTON_POSITION.x,
            MUSIC_BUTTON_POSITION.y + static_cast<int>(AUDIO_BUTTON_WIDTH) + BUTTON_GAP_Y
        };
    }
}

MiniMenuState::MiniMenuState(sf::RenderWindow& window, std::function<void()> onExitGame, std::function<void()> onRestartGame)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("mini_menu_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f - 117.f }, MENU_BUTTON_WIDTH, MENU_GAP)
    , m_volumeButton(MUSIC_BUTTON_POSITION, AUDIO_BUTTON_WIDTH) // כפתור מוזיקה (עליון)
    , m_soundOnButton(soundButtonPosition(), AUDIO_BUTTON_WIDTH)  // כפתור סאונד (תחתון)
    , m_onExitGame(std::move(onExitGame))
    , m_onRestartGame(std::move(onRestartGame))
{
    scaleAndCenterBackground();
    buildMenu();
}

void MiniMenuState::scaleAndCenterBackground()
{
    SpriteUtils::scaleAndCenter(m_background, m_window.getSize(), POPUP_WIDTH);
}

void MiniMenuState::buildMenu()
{
    auto& am = AssetsManager::getInstance();
    const sf::Texture& buttonTexture = am.getTexture("GenericButton");
    const sf::Font& font = am.getFont("Lilita");

    m_menu.addButton(buttonTexture, [this]() { onResumeClicked(); }, font, "Resume");
    m_menu.addButton(buttonTexture, [this]() { onInstructionsClicked(); }, font, "Instructions");
    m_menu.addButton(buttonTexture, [this]() { onRestartClicked(); }, font, "Restart Game");
    m_menu.addButton(buttonTexture, [this]() { onExitClicked(); }, font, "Exit Game");
}

void MiniMenuState::onResumeClicked()
{
    transitionTo(); // pop back to the paused GameplayState beneath this one
}

void MiniMenuState::onInstructionsClicked()
{
    pushState(std::make_unique<InstructionsState>(m_window));
}

void MiniMenuState::onRestartClicked()
{
    // Marks the paused GameplayState beneath this one as finished (with a
    // fresh replacement already queued), then pops this state. Controller
    // picks up the already-finished GameplayState the next time it becomes
    // top-of-stack, using its normal per-frame isFinished() check - no
    // special multi-level stack operation involved.
    m_onRestartGame();
    transitionTo();
}

void MiniMenuState::onExitClicked()
{
    // Same mechanism as Restart, just without a replacement queued, so the
    // still-alive MenuState beneath GameplayState ends up on top once both
    // finish.
    m_onExitGame();
    transitionTo();
}

void MiniMenuState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    m_menu.draw(window);
    m_volumeButton.draw(window);
    m_soundOnButton.draw(window);
}

void MiniMenuState::update(sf::Time deltaTime)
{
    // Nothing to animate on the mini menu.
}

void MiniMenuState::handleEvent(const sf::Event& event)
{
    //handleCloseEvent(event);
    //if (isTransitioning())
    //    return;

    m_menu.handleEvent(event);
    m_volumeButton.handleEvent(event);
    m_soundOnButton.handleEvent(event);
}
//
//