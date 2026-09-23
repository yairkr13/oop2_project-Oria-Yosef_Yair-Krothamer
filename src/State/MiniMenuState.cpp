#include "State/MiniMenuState.h"
#include "State/InstructionsState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr unsigned int POPUP_WIDTH = 640;
    constexpr unsigned int MENU_BUTTON_WIDTH = 140;
    constexpr float MENU_GAP = 12.f;
    constexpr unsigned int AUDIO_BUTTON_WIDTH = 60;
    constexpr int BUTTON_GAP_Y = 12;

    // Offset from the window's own vertical center, Config-based instead of a fixed pixel value.
    constexpr float MENU_Y_OFFSET_FROM_CENTER = -117.f;

    // Offset from the window's own center, matching these buttons' old absolute position at 1280x720.
    constexpr sf::Vector2i MUSIC_BUTTON_OFFSET_FROM_CENTER = { -143, -132 };
    constexpr sf::Vector2i MUSIC_BUTTON_POSITION = {
        static_cast<int>(Config::WINDOW_WIDTH) / 2 + MUSIC_BUTTON_OFFSET_FROM_CENTER.x,
        static_cast<int>(Config::WINDOW_HEIGHT) / 2 + MUSIC_BUTTON_OFFSET_FROM_CENTER.y
    };
    constexpr sf::Vector2i SOUND_BUTTON_POSITION = {
        MUSIC_BUTTON_POSITION.x,
        MUSIC_BUTTON_POSITION.y + static_cast<int>(AUDIO_BUTTON_WIDTH) + BUTTON_GAP_Y
    };
}

MiniMenuState::MiniMenuState(sf::RenderWindow& window, std::function<void()> onExitGame, std::function<void()> onRestartGame)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("mini_menu_bg"))
    , m_menu({ static_cast<float>(Config::WINDOW_WIDTH) / 2.f, static_cast<float>(Config::WINDOW_HEIGHT) / 2.f - 117.f }, MENU_BUTTON_WIDTH, MENU_GAP)
    , m_volumeButton(MUSIC_BUTTON_POSITION, AUDIO_BUTTON_WIDTH) // top
    , m_soundOnButton(SOUND_BUTTON_POSITION, AUDIO_BUTTON_WIDTH) // bottom
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

    if (m_onRestartGame)
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
    // Marks the paused GameplayState as finished (with a replacement queued), then pops this state.
    m_onRestartGame();
    transitionTo();
}

void MiniMenuState::onExitClicked()
{
    // Same mechanism as Restart, just without a replacement queued.
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
    m_menu.handleEvent(event);
    m_volumeButton.handleEvent(event);
    m_soundOnButton.handleEvent(event);
}