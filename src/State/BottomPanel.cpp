#include "State/BottomPanel.h"
#include "Constants.h"

BottomPanel::BottomPanel(const sf::Font& font)
    : m_endTurnHintText(font)
{
    // Matches the same windowHeight - BOTTOM_PANEL_HEIGHT computation
    // Player::BOTTOM_PANEL_TOP_Y uses, so the panel Player draws cards onto
    // and this one always agree on where it actually is.
    m_panel.setSize({ static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT });
    m_panel.setPosition({ 0.f, static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT });
    m_panel.setFillColor(sf::Color(40, 40, 40));

    m_endTurnHintText.setString("PRESS SPACE TO END TURN");
    m_endTurnHintText.setCharacterSize(13); // גודל קטן
    m_endTurnHintText.setFillColor(sf::Color(200, 200, 200, 180)); // צבע אפור-לבן עדין מעט שקוף

    // מיקום במרכז X ובחלק התחתון ביותר של המסך (12 פיקסלים מהקצה)
    float centerX = static_cast<float>(Config::WINDOW_WIDTH) / 2.0f;
    float bottomY = static_cast<float>(Config::WINDOW_HEIGHT) - 12.0f;
    m_endTurnHintText.setPosition({ centerX, bottomY });
}

void BottomPanel::draw(sf::RenderWindow& window) const
{
    window.draw(m_panel);
    window.draw(m_endTurnHintText);
}
