#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <optional>

// A clickable, hoverable sprite with an optional text label - the base
// widget every Menu button and audio toggle button in the UI is built from.
class Button
{
public:
    using Func = std::function<void()>;

    Button(sf::IntRect rect, const sf::Texture& texture, Func func, sf::Vector2f scale = { 1.f, 1.f });

    Button(sf::IntRect rect, const sf::Texture& texture, Func func,
        const sf::Font& font, const std::string& label, sf::Vector2f scale = { 1.f, 1.f });

    static Button fromTextureWidth(sf::Vector2i position, const sf::Texture& texture, unsigned int width, Func func);

	void handleEvent(const sf::Event& event );

    void draw(sf::RenderWindow& window) const;

    void setTexture(const sf::Texture& texture);

private:
    void initSprite(sf::IntRect rect);

    void setHovered(bool hovered);

    bool m_isHovered = false;
    sf::IntRect m_rect;
	sf::Sprite m_sprite;
    Func m_func;
    sf::Vector2f m_scale;
    std::optional<sf::Text> m_label;
    void handle(const sf::Event::MouseButtonPressed& event);
    void handle(const sf::Event::MouseMoved& event);
    void handle(const auto& event) {};
};