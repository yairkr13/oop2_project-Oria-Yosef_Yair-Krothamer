#include "State/LoadingState.h"
#include "State/MenuState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include <algorithm>

namespace
{
    // Degrees per second the spinner rotates at.
    constexpr float SPINNER_ROTATION_SPEED = 240.f;

    // On-screen size (max dimension, in pixels), independent of the source texture's resolution.
    constexpr float SPINNER_DISPLAY_SIZE = 72.f;

    // Max time per frame spent loading queued assets - batches cheap loads
    // together without changing how long any single asset's own load takes.
    const sf::Time LOAD_TIME_BUDGET = sf::milliseconds(4);
}

LoadingState::LoadingState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("await_bg"))
    , m_spinner(AssetsManager::getInstance().getTexture("spinner"))
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());

    // Origin centered so rotate() spins it in place; positioned at window center.
    sf::Vector2u spinnerSize = m_spinner.getTexture().getSize();
    m_spinner.setOrigin({ spinnerSize.x / 2.f, spinnerSize.y / 2.f });
    m_spinner.setPosition({ static_cast<float>(m_window.getSize().x) / 2.f,
                             static_cast<float>(m_window.getSize().y) / 2.f });

    // Uniform scale to SPINNER_DISPLAY_SIZE, independent of the texture's native resolution.
    float spinnerScale = SpriteUtils::maxDimensionScale(spinnerSize, SPINNER_DISPLAY_SIZE);
    m_spinner.setScale({ spinnerScale, spinnerScale });

    // Boot assets are already loaded; queue the rest for update() to drain incrementally.
    AssetsManager::getInstance().queueRemainingAssets();
}

void LoadingState::update(sf::Time deltaTime)
{
    m_spinner.rotate(sf::degrees(SPINNER_ROTATION_SPEED * deltaTime.asSeconds()));

    // Loads assets one at a time until LOAD_TIME_BUDGET is spent; always
    // loads at least one, so every frame makes forward progress.
    sf::Clock frameLoadClock;
    bool hasMore;
    do
    {
        hasMore = AssetsManager::getInstance().loadNext();
    } while (hasMore && frameLoadClock.getElapsedTime() < LOAD_TIME_BUDGET);

    if (!hasMore)
        transitionTo(std::make_unique<MenuState>(m_window));
}

void LoadingState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    window.draw(m_spinner);
}

void LoadingState::handleEvent(const sf::Event& event)
{
    // Nothing to interact with while loading.
}
