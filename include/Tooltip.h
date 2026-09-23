#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Constants.h"

// A small text box shown next to the mouse cursor (e.g. hovering a Card or
// a board tile) - stays hidden until show() is called with non-empty text.
class Tooltip {
public:
    explicit Tooltip(const sf::Font& font);

    void show(const std::string& text, const sf::Vector2f& mousePos);
    void hide();

    void draw(sf::RenderWindow& window) const;

private:
    sf::Text m_text;
    sf::RectangleShape m_background;
    bool m_visible = false;
};