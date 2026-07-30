#include "Menu.h"

void Menu::addButton(Button& button)
{
	//??? ????? ?????? ????? ?????
	//
}

void Menu::handleEvent(const sf::Event& event)
{
	for (auto& button : m_buttons)
	{
		button.handleEvent(event);
	}
	event.visit([this](const auto& e) { handle(e); });
}

void Menu::draw(sf::RenderWindow& window) const
{
	for (auto& button: m_buttons)
	{
		button.draw(window);
	}
}

void Menu::handle(const sf::Event::KeyPressed& event)
{
	
}