#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include "Player.h"
#include "MainMenu.h"

enum class GameState { MainMenu, Playing };

class Game
{
public:
    Game();
    void run();

private:
    sf::RenderWindow         m_window;
    GameState                m_state;
    std::optional<MainMenu>  m_menu;   // optional so we can catch font errors

    std::unique_ptr<Player>          m_player;
    std::vector<sf::RectangleShape>  m_obstacles;
    sf::Clock  m_clock;
    sf::View   m_view;

    std::unordered_map<std::string, sf::Texture> m_textures;
    void loadTextures();

    void handleEvents();
    void update(float dt);
    void draw();

    static std::vector<sf::RectangleShape> makeObstacles();
};
