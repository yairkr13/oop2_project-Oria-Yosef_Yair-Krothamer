#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <optional>

class Button
{
public:
    using Func = std::function<void()>;

    // `scale` is the base scale applied to the sprite (e.g. to fit a large
    // texture into a smaller on-screen rect). Callers that don't need scaling
    // can omit it. Hover feedback multiplies on top of this base scale.
    Button(sf::IntRect rect, const sf::Texture& texture, Func func, sf::Vector2f scale = { 1.f, 1.f });

    // Same as above, plus a text caption centered over the sprite - for
    // buttons that share a generic texture (no baked-in label) and need
    // their own text drawn on top instead.
    Button(sf::IntRect rect, const sf::Texture& texture, Func func,
        const sf::Font& font, const std::string& label, sf::Vector2f scale = { 1.f, 1.f });

    // Builds a Button positioned at `position`, sized so `texture`'s width
    // matches `width` (aspect preserved) - the "scale a button to a target
    // on-screen width" computation every menu/HUD button-builder (e.g.
    // Menu::nextButtonRect) otherwise re-derives by hand. `func` is the
    // click handler, forwarded straight through to the normal constructor.
    static Button fromTextureWidth(sf::Vector2i position, const sf::Texture& texture, unsigned int width, Func func);

	void handleEvent(const sf::Event& event );

    void draw(sf::RenderWindow& window) const;

    // Swaps the displayed texture without changing rect/position/scale -
    // for toggle-style buttons (e.g. the music icon).
    void setTexture(const sf::Texture& texture);

private:
    void initSprite(sf::IntRect rect);

    // Only ever called from this class's own handle(MouseMoved) below - no
    // external caller (Menu, AudioToggleButton, any State) sets hover state directly.
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