#include "State/MenuState.h"
#include "State/GameModeState.h"
#include "State/InstructionsState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"

MenuState::MenuState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("menu_bg"))
    , m_menu({ static_cast<float>(window.getSize().x) / 2.f, 350.f }, 340, 22.f)
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
}

void MenuState::update(sf::Time deltaTime)
{
    // Nothing to animate in the main menu.
}

void MenuState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);
}
