#include "State/InstructionsState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include "Constants.h"
#include <algorithm>

namespace
{
    constexpr unsigned int NAV_BUTTON_WIDTH = 70;
    constexpr int NAV_BUTTON_MARGIN = 30; // distance from the left/right edge of the window
    constexpr int NAV_BUTTON_Y = static_cast<int>(Config::WINDOW_HEIGHT) / 2 - 35; // roughly vertical center

    const sf::Vector2i PREV_BUTTON_POSITION = { NAV_BUTTON_MARGIN, NAV_BUTTON_Y };
    const sf::Vector2i NEXT_BUTTON_POSITION = {
        static_cast<int>(Config::WINDOW_WIDTH) - NAV_BUTTON_MARGIN - static_cast<int>(NAV_BUTTON_WIDTH),
        NAV_BUTTON_Y
    };

    // Instructions1/2/3.png's own AssetsManager keys, indexed by page - the
    // single place that maps a page index to its texture (see
    // InstructionsState::setPage).
    constexpr const char* PAGE_TEXTURE_KEYS[3] = { "instructions_bg_1", "instructions_bg_2", "instructions_bg_3" };
}

InstructionsState::InstructionsState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture(PAGE_TEXTURE_KEYS[0]))
    , m_menu({ 90.f, 20.f }, 140, 0.f) // single button, top-left corner
    , m_nextPageButton(Button::fromTextureWidth(NEXT_BUTTON_POSITION,
          AssetsManager::getInstance().getTexture("NextMenuButton"), NAV_BUTTON_WIDTH,
          [this]() { goToNextPage(); }))
    , m_prevPageButton(Button::fromTextureWidth(PREV_BUTTON_POSITION,
          AssetsManager::getInstance().getTexture("BackMenuButton"), NAV_BUTTON_WIDTH,
          [this]() { goToPreviousPage(); }))
{
    scaleBackgroundToWindow();
    buildMenu();
}

void InstructionsState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}

void InstructionsState::buildMenu()
{
    m_menu.addButton(AssetsManager::getInstance().getTexture("BackButton"),
        [this]() { onBackClicked(); });
}

void InstructionsState::onBackClicked()
{
    transitionTo(); // pop back to whichever state pushed this one
}

void InstructionsState::setPage(int page)
{
    m_page = std::clamp(page, 0, 2);
    m_background.setTexture(AssetsManager::getInstance().getTexture(PAGE_TEXTURE_KEYS[m_page]), true);
    scaleBackgroundToWindow();
}

void InstructionsState::goToNextPage()
{
    setPage(m_page + 1);
}

void InstructionsState::goToPreviousPage()
{
    setPage(m_page - 1);
}

void InstructionsState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    m_menu.draw(window);

    if (m_page > 0)
        m_prevPageButton.draw(window);
    if (m_page < 2)
        m_nextPageButton.draw(window);
}

void InstructionsState::update(sf::Time deltaTime)
{
    // Nothing to animate on the instructions screen.
}

void InstructionsState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);

    if (m_page > 0)
        m_prevPageButton.handleEvent(event);
    if (m_page < 2)
        m_nextPageButton.handleEvent(event);

    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            onBackClicked(); // חזרה למסך הקודם
        }
    }
}
