#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include "Button.h"

// Lays out and scales buttons automatically: each texture is scaled to
// `buttonWidth`, stacked vertically, and centered on `center.x`. Callers
// just supply textures and callbacks - no size/position math needed.
class Menu
{
public:
	Menu(sf::Vector2f center, unsigned int buttonWidth, float gap);

	void addButton(const sf::Texture& texture, Button::Func func);

    // Sized to `width` instead of the Menu's own buttonWidth - for a texture
    // whose aspect ratio differs enough from the others sharing this Menu
    // that the shared width would throw off the stack.
    void addButton(const sf::Texture& texture, Button::Func func, unsigned int width);

    // Same layout, plus a text caption centered over the button.
    void addButton(const sf::Texture& texture, Button::Func func, const sf::Font& font, const std::string& label);
	void handleEvent(const sf::Event& event);
	void draw(sf::RenderWindow& window) const;

private:
	std::pair<sf::IntRect, sf::Vector2f> nextButtonRect(const sf::Texture& texture, unsigned int width);

	std::vector<Button> m_buttons;
	sf::Vector2f m_center;
	unsigned int m_buttonWidth;
	float m_gap;
	float m_nextY;

	void handle(const sf::Event::KeyPressed& event);
	void handle(const auto& event) {};
};
