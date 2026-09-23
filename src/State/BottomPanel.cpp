#include "State/BottomPanel.h"
#include "Constants.h"

BottomPanel::BottomPanel(const sf::Font& font)
    : m_endTurnHintText(font)
{
    // Must match Player::BOTTOM_PANEL_TOP_Y so the panel and the cards drawn onto it agree.
    m_panel.setSize({ static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT });
    m_panel.setPosition({ 0.f, static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT });
    m_panel.setFillColor(sf::Color(40, 40, 40)); // dark gray

    m_endTurnHintText.setString("PRESS SPACE TO END TURN");
    m_endTurnHintText.setCharacterSize(13);
    m_endTurnHintText.setFillColor(sf::Color(200, 200, 200, 180)); // soft translucent gray-white

    // Origin's X centered so setPosition below centers the text horizontally.
    sf::FloatRect bounds = m_endTurnHintText.getLocalBounds();
    m_endTurnHintText.setOrigin({ bounds.position.x + bounds.size.x / 2.f, 0.f });

    // Centered horizontally, near the bottom edge.
    float centerX = static_cast<float>(Config::WINDOW_WIDTH) / 2.0f;
    float bottomY = static_cast<float>(Config::WINDOW_HEIGHT) - 12.0f;
    m_endTurnHintText.setPosition({ centerX, bottomY });
}

void BottomPanel::draw(sf::RenderWindow& window) const
{
    window.draw(m_panel);
    window.draw(m_endTurnHintText);
}
