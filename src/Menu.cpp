#include "Menu.h"

void Menu::addButton(Button& button)
{
}

void Menu::handleEvent(const sf::Event& event)
{
	event.visit([this](const auto& e) { handle(e); });
}

void Menu::draw(sf::RenderWindow& window) const
{
	
}

void Menu::handle(const sf::Event::MouseButtonPressed& event)
{
	
}

void Menu::handle(const sf::Event::KeyPressed& event)
{
	
}

void Menu::handle(const sf::Event::MouseMoved& event)
{
	
}

void Menu::handle(const auto& event) {}