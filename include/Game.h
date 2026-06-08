#pragma once
#include "SFML/graphics.hpp"

class Game
{
public:
	Game();
	void run();

	enum class State { Menu, Playing, LevelSolved, GameComplete };
private: 
	sf::RenderWindow m_window;
	void handle(const sf::Event::Closed& event);
	void handle(const sf::Event::MouseButtonPressed& event);
	void handle(const sf::Event::KeyPressed& event);
	void handle(const auto& event) {};
	void draw();
};