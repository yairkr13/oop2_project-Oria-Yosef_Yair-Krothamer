#pragma once
#include "SFML/Graphics.hpp"

class Player
{
public:
	Player();
	void update();
	void draw(sf::RenderWindow& window);
private:
	sf::Vector2f m_position;
/* 
	sf::Sprite m_sprite;
	sf::Texture m_texture;*/
};