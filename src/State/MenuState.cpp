#include "State/MenuState.h"
#include "State/GameModeState.h"
#include "State/InstructionsState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr unsigned int AUDIO_BUTTON_WIDTH = 60;
    constexpr int AUDIO_BUTTON_TOP_MARGIN = 20;
    constexpr int BUTTON_GAP = 15; // gap between the two audio buttons

    // Offset from the window's own vertical center, Config-based instead of a fixed pixel value.
    constexpr float MENU_Y_OFFSET_FROM_CENTER = -10.f;

    // Music button position (left of the pair).
    sf::Vector2i musicButtonPosition(const sf::RenderWindow& window)
    {
        int centerX = static_cast<int>(window.getSize().x) / 2;
        int totalWidth = (AUDIO_BUTTON_WIDTH * 2) + BUTTON_GAP;
        int startX = centerX - (totalWidth / 2);

        return { startX, AUDIO_BUTTON_TOP_MARGIN };
    }

    // Sound button position (right of the music button).
    sf::Vector2i soundButtonPosition(const sf::RenderWindow& window)
    {
        sf::Vector2i musicPos = musicButtonPosition(window);
        return { musicPos.x + static_cast<int>(AUDIO_BUTTON_WIDTH) + BUTTON_GAP, AUDIO_BUTTON_TOP_MARGIN };
    }
}

MenuState::MenuState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("menu_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + MENU_Y_OFFSET_FROM_CENTER }, 340, 22.f)
    , m_volumeButton(musicButtonPosition(window), AUDIO_BUTTON_WIDTH)
    , m_soundOnButton(soundButtonPosition(window), AUDIO_BUTTON_WIDTH)
{
    scaleBackgroundToWindow();
    buildMenu();
}

void MenuState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}

void MenuState::buildMenu()
{
    auto& am = AssetsManager::getInstance();

    m_menu.addButton(am.getTexture("StartGameButton"), [this]() { onStartGameClicked(); });
    m_menu.addButton(am.getTexture("InstructionsButton"), [this]() { onInstructionsClicked(); });
    m_menu.addButton(am.getTexture("ExitButton"), [this]() { onExitClicked(); });
}

void MenuState::onStartGameClicked()
{
    pushState(std::make_unique<GameModeState>(m_window));
}

void MenuState::onInstructionsClicked()
{
    pushState(std::make_unique<InstructionsState>(m_window));
}

void MenuState::onExitClicked()
{
    transitionTo(); // no successor - Controller closes the window.
}

void MenuState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    m_menu.draw(window);
    m_volumeButton.draw(window);
    m_soundOnButton.draw(window);
}

void MenuState::update(sf::Time deltaTime)
{
    // Nothing to animate in the main menu.
}

void MenuState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);
    m_volumeButton.handleEvent(event);
    m_soundOnButton.handleEvent(event);
}

MusicTrack MenuState::desiredMusicTrack() const
{
    return MusicTrack::Menu;
}
