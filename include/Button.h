#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button
{
public:
    using Func = std::function<void()>;

    Button(sf::IntRect rect, const sf::Texture& texture, Func func);
    
	void handleEvent(const sf::Event& event );
    //void click(sf::Vector2i clickPos);
    //void hover(sf::Vector2i mousePos);

    void draw(sf::RenderWindow& window) const;
	void setHovered(bool hovered);
    

private:
    sf::IntRect m_rect;
    //sf::Text m_text;
	sf::Sprite m_sprite;
    Func m_func;
    void handle(const sf::Event::MouseButtonPressed& event);
    void handle(const sf::Event::MouseMoved& event);
    void handle(const auto& event) {};
};