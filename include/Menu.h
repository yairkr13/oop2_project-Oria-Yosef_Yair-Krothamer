#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"

class Menu
{
public:
	void addButton(Button& button);
	void handleEvent(const sf::Event& event);
	void draw(sf::RenderWindow& window) const;

private:
	std::vector<Button> m_buttons;
	void handle(const sf::Event::KeyPressed& event);
	void handle(const auto& event) {};
};