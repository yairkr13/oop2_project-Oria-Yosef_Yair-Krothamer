#include "Menu.h"

Menu::Menu(sf::Vector2f center, unsigned int buttonWidth, float gap)
	: m_center(center), m_buttonWidth(buttonWidth), m_gap(gap), m_nextY(center.y)
{
}

void Menu::addButton(const sf::Texture& texture, Button::Func func)
{
	auto [rect, scale] = nextButtonRect(texture);
	m_buttons.emplace_back(rect, texture, func, scale);
}

void Menu::addButton(const sf::Texture& texture, Button::Func func, const sf::Font& font, const std::string& label)
{
	auto [rect, scale] = nextButtonRect(texture);
	m_buttons.emplace_back(rect, texture, func, font, label, scale);
}

std::pair<sf::IntRect, sf::Vector2f> Menu::nextButtonRect(const sf::Texture& texture)
{
	auto textureSize = texture.getSize();
	float scale = static_cast<float>(m_buttonWidth) / static_cast<float>(textureSize.x);
	float scaledHeight = static_cast<float>(textureSize.y) * scale;

	sf::Vector2i size(static_cast<int>(m_buttonWidth), static_cast<int>(scaledHeight));
	sf::Vector2i position(static_cast<int>(m_center.x - m_buttonWidth / 2.f), static_cast<int>(m_nextY));

	m_nextY += scaledHeight + m_gap;

	return { sf::IntRect(position, size), sf::Vector2f{ scale, scale } };
}

void Menu::handleEvent(const sf::Event& event)
{
	for (auto& button : m_buttons)
	{
		button.handleEvent(event);
	}
	event.visit([this](const auto& e) { handle(e); });
}

void Menu::draw(sf::RenderWindow& window) const
{
	for (auto& button: m_buttons)
	{
		button.draw(window);
	}
}

void Menu::handle(const sf::Event::KeyPressed& event)
{
	// Handle key press events if needed
}
