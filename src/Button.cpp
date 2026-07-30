#include "button.h"

Button::Button(sf::IntRect rect, const sf::Texture& texture, Func func )
    : m_rect(rect), m_func(func), m_sprite(texture)
{
	m_sprite.setOrigin({
			m_sprite.getLocalBounds().size.x / 2.f,
			m_sprite.getLocalBounds().size.y / 2.f
		});
	m_sprite.setPosition({
		static_cast<float>(rect.getCenter().x),
		static_cast<float>(rect.getCenter().y)
		});
}

void Button::handleEvent(const sf::Event& event)
{
	event.visit([this](const auto& e) { handle(e); });
}

void Button::draw(sf::RenderWindow& window) const
{
	window.draw(m_sprite);
}
void Button::setHovered(bool hovered)
{
	m_sprite.setScale(hovered ? sf::Vector2f{1.1f, 1.1f} : sf::Vector2f{1.f, 1.f});
}

void Button::handle(const sf::Event::MouseButtonPressed& event)
{
	if (event.button == sf::Mouse::Button::Left)
	{
		sf::Vector2i clickPos(event.position.x, event.position.y);
		if (m_rect.contains(clickPos))
		{
			m_func();
		}
	}
}

void Button::handle(const sf::Event::MouseMoved& event)
{
	sf::Vector2i mousePos(event.position.x, event.position.y);
	setHovered(m_rect.contains(mousePos));
}