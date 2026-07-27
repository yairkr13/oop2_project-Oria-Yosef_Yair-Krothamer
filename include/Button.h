#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button
{
public:
    using Func = std::function<void()>;

    Button(sf::IntRect rect, Func func, sf::Texture& texture);
    
	void handleEvent(const sf::Event& event );
    //void click(sf::Vector2i clickPos);
    //void hover(sf::Vector2i mousePos);

    void draw(sf::RenderWindow& window) const;
	void setHovered(bool hovered);
    

private:
    sf::IntRect m_rect;
    sf::Text m_text;
	sf::Texture& m_texture;
    Func m_func;

	bool m_hovered = false;
};