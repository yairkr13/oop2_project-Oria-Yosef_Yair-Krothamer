#include "State/InstructionsState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"

InstructionsState::InstructionsState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("instructions_bg"))
    , m_menu({ 90.f, 20.f }, 140, 0.f) // single button, top-left corner
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

void InstructionsState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    m_menu.draw(window);
}

void InstructionsState::update(sf::Time deltaTime)
{
    // Nothing to animate on the instructions screen.
}

void InstructionsState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);
}
