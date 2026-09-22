#pragma once
#include <SFML/Graphics.hpp>

// The gameplay screen's bottom HUD strip: the dark background panel behind
// each player's hand of cards, plus the "PRESS SPACE TO END TURN" hint
// centered along its bottom edge. Pure presentation - owns no gameplay
// state, knows nothing about Board/Player/Card/turns, just draws itself.
// GameplayState still draws each player's hand of cards on top of this
// separately (see Player::draw) - this only owns the panel/hint behind it.
class BottomPanel
{
public:
    explicit BottomPanel(const sf::Font& font);

    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_panel;
    sf::Text m_endTurnHintText;
};
