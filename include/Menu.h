#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include "Button.h"

// Lays out and scales buttons automatically: each texture added is scaled so
// its on-screen width matches `buttonWidth` (aspect ratio preserved), stacked
// vertically below the previous button and horizontally centered on
// `center.x`. Callers just supply textures and callbacks - no size or
// position math required.
class Menu
{
public:
	Menu(sf::Vector2f center, unsigned int buttonWidth, float gap);

	void addButton(const sf::Texture& texture, Button::Func func);

    // Same layout, plus a text caption centered over the button - for
    // buttons sharing a generic texture instead of a pre-labeled one.
    void addButton(const sf::Texture& texture, Button::Func func, const sf::Font& font, const std::string& label);
	void handleEvent(const sf::Event& event);
	void draw(sf::RenderWindow& window) const;

private:
	// Computes the next stacked button's rect/scale and advances m_nextY -
	// shared layout math for both addButton overloads.
	std::pair<sf::IntRect, sf::Vector2f> nextButtonRect(const sf::Texture& texture);

	std::vector<Button> m_buttons;
	sf::Vector2f m_center;
	unsigned int m_buttonWidth;
	float m_gap;
	float m_nextY;

	void handle(const sf::Event::KeyPressed& event);
	void handle(const auto& event) {};
};
