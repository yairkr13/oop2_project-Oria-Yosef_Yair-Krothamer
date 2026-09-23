#pragma once
#include <SFML/Graphics.hpp>

// The gameplay screen's bottom HUD strip: dark panel plus the "PRESS SPACE
// TO END TURN" hint. Pure presentation - owns no gameplay state; Player
// draws each hand of cards on top of this separately.
class BottomPanel
{
public:
    explicit BottomPanel(const sf::Font& font);

    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_panel;
    sf::Text m_endTurnHintText;
};
