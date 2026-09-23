#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Constants.h"

class Tooltip {
public:
    explicit Tooltip(const sf::Font& font);

    void show(const std::string& text, const sf::Vector2f& mousePos);
    //void show(const std::string& text, const sf::Vector2f& mousePos) {
    //    if (text.empty()) {
    //        m_visible = false;
    //        return;
    //    }

    //    m_text.setString(text);

    //    float padding = 8.f;
    //    sf::FloatRect textBounds = m_text.getLocalBounds();

    //    // SFML 3: שימוש ב-size.x/y במקום width/height
    //    m_background.setSize({ textBounds.size.x + padding * 2.f, textBounds.size.y + padding * 2.f });

    //    sf::Vector2f offset(15.f, 15.f);
    //    sf::Vector2f finalPos = mousePos + offset;

    //    m_background.setPosition(finalPos);

    //    // SFML 3: שימוש ב-position.x/y במקום left/top
    //    m_text.setPosition({ finalPos.x + padding - textBounds.position.x,
    //                         finalPos.y + padding - textBounds.position.y });

    //    m_visible = true;
    //}

    void hide();

    void draw(sf::RenderWindow& window) const;

private:
    sf::Text m_text;
    sf::RectangleShape m_background;
    bool m_visible = false;
};