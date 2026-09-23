#include "button.h"
#include "SoundPlayer.h"
#include "SpriteUtils.h"

Button::Button(sf::IntRect rect, const sf::Texture& texture, Func func, sf::Vector2f scale)
    : m_rect(rect), m_func(func), m_sprite(texture), m_scale(scale)
{
    initSprite(rect);
}

Button Button::fromTextureWidth(sf::Vector2i position, const sf::Texture& texture, unsigned int width, Func func)
{
    auto textureSize = texture.getSize();
    float scale = SpriteUtils::widthScale(textureSize, width);
    int scaledHeight = static_cast<int>(textureSize.y * scale);

    sf::IntRect rect(position, { static_cast<int>(width), scaledHeight });
    return Button(rect, texture, std::move(func), sf::Vector2f{ scale, scale });
}

Button::Button(sf::IntRect rect, const sf::Texture& texture, Func func,
    const sf::Font& font, const std::string& label, sf::Vector2f scale)
    : m_rect(rect), m_func(func), m_sprite(texture), m_scale(scale), m_label(std::in_place, font)
{
    initSprite(rect);

    m_label->setString(label);
    m_label->setCharacterSize(22);
    m_label->setFillColor(sf::Color::White);

    auto bounds = m_label->getLocalBounds();
    m_label->setOrigin({ bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
    m_label->setPosition({
        static_cast<float>(rect.getCenter().x),
        static_cast<float>(rect.getCenter().y)
        });
}

void Button::initSprite(sf::IntRect rect)
{
	m_sprite.setOrigin({
			m_sprite.getLocalBounds().size.x / 2.f,
			m_sprite.getLocalBounds().size.y / 2.f
		});
	m_sprite.setPosition({
		static_cast<float>(rect.getCenter().x),
		static_cast<float>(rect.getCenter().y)
		});
	m_sprite.setScale(m_scale);
}

void Button::handleEvent(const sf::Event& event)
{
	event.visit([this](const auto& e) { handle(e); });
}

void Button::draw(sf::RenderWindow& window) const
{
	window.draw(m_sprite);
    if (m_label)
        window.draw(*m_label);
}
void Button::setHovered(bool hovered)
{
	if (m_isHovered == hovered)
		return;

	m_isHovered = hovered;

	// הצליל מושמע רק ברגע הכניסה לכפתור (מעבר מ-false ל-true)
	if (m_isHovered)
	{
		SoundPlayer::getInstance().play("hover_on_button");
	}
	
	float hoverFactor = hovered ? 1.1f : 1.f;
	m_sprite.setScale({ m_scale.x * hoverFactor, m_scale.y * hoverFactor });
}

void Button::setTexture(const sf::Texture& texture)
{
    m_sprite.setTexture(texture);
}

void Button::handle(const sf::Event::MouseButtonPressed& event)
{
	if (event.button == sf::Mouse::Button::Left)
	{
		sf::Vector2i clickPos(event.position.x, event.position.y);
		// click only counts if it lands inside this button's rect, checked independently of hover state
		if (m_rect.contains(clickPos))
		{
			SoundPlayer::getInstance().play("button_click");
			if (m_func) // בדיקה שה-std::function אינו ריק
			{
				m_func();
			}
		}
	}
}

void Button::handle(const sf::Event::MouseMoved& event)
{
	sf::Vector2i mousePos(event.position.x, event.position.y);
	setHovered(m_rect.contains(mousePos));
}