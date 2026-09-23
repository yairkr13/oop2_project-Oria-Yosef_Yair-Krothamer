#include "Tooltip.h"

Tooltip::Tooltip(const sf::Font& font)
    : m_text(font) // SFML 3 requires sf::Text to be constructed with a font
{
    m_text.setCharacterSize(14);
    m_text.setFillColor(sf::Color::White);

    m_background.setFillColor(sf::Color(20, 20, 30, 230)); // near-black
    m_background.setOutlineColor(sf::Color(180, 180, 210)); // light gray
    m_background.setOutlineThickness(1.5f);
}

// Positions the box near the cursor, flipped to whichever side keeps it
// fully on screen.
void Tooltip::show(const std::string& text, const sf::Vector2f& mousePos) {
    if (text.empty()) {
        m_visible = false;
        return;
    }

    m_text.setString(text);

    float padding = 8.f;
    sf::FloatRect textBounds = m_text.getLocalBounds();
    sf::Vector2f tooltipSize = { textBounds.size.x + padding * 2.f, textBounds.size.y + padding * 2.f };
    m_background.setSize(tooltipSize);

    constexpr float windowWidth = static_cast<float>(Config::WINDOW_WIDTH);
    constexpr float windowHeight = static_cast<float>(Config::WINDOW_HEIGHT);

    sf::Vector2f finalPos = mousePos + sf::Vector2f(15.f, 15.f);

    // Flip to the cursor's left/above if it would overflow that edge.
    if (finalPos.x + tooltipSize.x > windowWidth) {
        finalPos.x = mousePos.x - tooltipSize.x - 15.f;
    }
    if (finalPos.y + tooltipSize.y > windowHeight) {
        finalPos.y = mousePos.y - tooltipSize.y - 15.f;
    }

    m_background.setPosition(finalPos);
    m_text.setPosition({ finalPos.x + padding - textBounds.position.x,
                         finalPos.y + padding - textBounds.position.y });

    m_visible = true;
}

void Tooltip::hide() {
    m_visible = false;
}

void Tooltip::draw(sf::RenderWindow& window) const {
    if (m_visible) {
        window.draw(m_background);
        window.draw(m_text);
    }
}
