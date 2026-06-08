#include "Game.h"

Game::Game() : m_window(sf::VideoMode(800, 600), "SFML Game") {}

void Game::run()
{
    while (m_window.isOpen())
    {
		m_window.clear();
        draw();
        m_window.display();

        if (const auto event = m_window.waitEvent())
        {
            event->visit([this](const auto& e) { handle(e); });
        }
    }
}

void Game::handle(const sf::Event::Closed& event)
{
    m_window.close();
}

void handle(const sf::Event::MouseButtonPressed& event)
{
    switch(event.button.button)
    {
	case sf::Mouse::Left:

        // Handle left mouse button press
	}
}

void Game::draw()
{
    
    // Drawing code goes here
}