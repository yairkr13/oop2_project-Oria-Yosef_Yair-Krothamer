// Card.h
#pragma once
#include "Button.h"
#include "Monsters/Monster.h"
#include <memory>

class Card
{
public:
    Card(sf::IntRect rect, std::shared_ptr<Monster> monster, Button::Func onClick);

    void handleEvent(const sf::Event& event);
    void update(bool enoughKeys, bool isSelected);
    void draw(sf::RenderWindow& window) const;

    bool canInteract() const;

private:
    std::shared_ptr<Monster> m_monster;
    Button m_button; // 👈 הכפתור הפנימי שמשמש ככלי עזר
    sf::Vector2f m_position;

    bool m_isSelected = false;
    bool m_enoughKeys = true;
};