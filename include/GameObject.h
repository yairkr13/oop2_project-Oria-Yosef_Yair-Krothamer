#pragma once
#include "SFML/Graphics.hpp"

class GameObject
{
	//void draw(sf::RenderWindow& window) const;
	//sf::FloatRect getBound() const;
	//sf::Vector2f getPosition() const;
public:
	GameObject(const sf::Texture& texture, const sf::Vector2f& position);
	//d-tor
	virtual void draw(sf::RenderWindow& window) const = 0;
private:
	sf::Sprite m_sprite;
	sf::Vector2f m_position;
};