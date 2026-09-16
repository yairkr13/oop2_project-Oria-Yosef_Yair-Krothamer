#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Tooltip {
public:
    explicit Tooltip(const sf::Font& font)
        : m_text(font) // אתחול sf::Text ברשימת האתחול (חובה ב-SFML 3)
    {
        m_text.setCharacterSize(14);
        m_text.setFillColor(sf::Color::White);

        m_background.setFillColor(sf::Color(20, 20, 30, 230));
        m_background.setOutlineColor(sf::Color(180, 180, 210));
        m_background.setOutlineThickness(1.5f);
    }

    void show(const std::string& text, const sf::Vector2f& mousePos, float windowWidth = Config::WINDOW_WIDTH) {
        if (text.empty()) {
            m_visible = false;
            return;
        }

        m_text.setString(text);

        float padding = 8.f;
        sf::FloatRect textBounds = m_text.getLocalBounds();
        sf::Vector2f tooltipSize = { textBounds.size.x + padding * 2.f, textBounds.size.y + padding * 2.f };
        m_background.setSize(tooltipSize);

        // 1. ברירת מחדל: מיקום מימין ולמטה מהעכבר
        sf::Vector2f finalPos = mousePos + sf::Vector2f(15.f, 15.f);

        // 2. אם החריגה מגיעה אל מעבר לקצה הימני - נזיז אותו לשמאל העכבר!
        if (finalPos.x + tooltipSize.x > windowWidth) {
            finalPos.x = mousePos.x - tooltipSize.x - 15.f;
        }

        m_background.setPosition(finalPos);
        m_text.setPosition({ finalPos.x + padding - textBounds.position.x,
                             finalPos.y + padding - textBounds.position.y });

        m_visible = true;
    }
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

    void hide() {
        m_visible = false;
    }

    void draw(sf::RenderWindow& window) const {
        if (m_visible) {
            window.draw(m_background);
            window.draw(m_text);
        }
    }

private:
    sf::Text m_text;
    sf::RectangleShape m_background;
    bool m_visible = false;
};