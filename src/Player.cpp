#include "Player.h"

Player::Player() : m_position(100.f, 100.f) /*, m_texture(), m_sprite()*/
{
	// Load texture and set up sprite
	// m_texture.loadFromFile("player.png");
	// m_sprite.setTexture(m_texture);
	// m_sprite.setPosition(m_position);
	sf::RectangleShape shape(sf::Vector2f(20.f, 20.f));
	shape.setPosition(m_position);
	shape.setFillColor(sf::Color::Green);
}

void Player::update()
{

	// Handle player movement and game logic here
}

void Player::draw(sf::RenderWindow& window)
{
	window.draw(shape);
}