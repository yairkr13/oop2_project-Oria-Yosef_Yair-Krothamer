#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"

class Game
{
public:
    Game();
    void run();

private:
    sf::RenderWindow m_window;
    Player m_player;
    std::vector<sf::RectangleShape> m_obstacles;
    sf::Clock m_clock;
    sf::View m_view;

    std::unordered_map<std::string, sf::Texture> m_textures;
    void loadTextures();

    void handleEvents();
    void update(float dt);
    void draw();

    static std::vector<sf::RectangleShape> makeObstacles();
};
