#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Board.h"

enum class GameState { MainMenu, Playing }; //turn1,turn2
enum class TurnState 
{
    ChoosingMonster,
    ChoosingTargetTile,
    MonsterSelected,
    Attacking,
    TurnEnded
};

class Game
{
public:
    Game();
    void run();
    //void setState(GameState newState);

private:
    sf::RenderWindow m_window;
    GameState m_state;
    TurnState m_turnState;
    //std::optional<MainMenu> m_menu;   // optional so we can catch font errors

    std::unique_ptr<Player> m_player1;
	std::unique_ptr<Player> m_player2;
    Player* m_currentPlayer;

    sf::Clock m_clock;
	//timer for every turn, when time is up- switch to the other player - 2:00
    // 
    //sf::View m_view; //when its player 1 turn- the screen get closer to player 1

    //std::unordered_map<std::string, sf::Texture> m_textures;
    //void loadTextures();

    void handleEvents();
    void update(float dt);
    void draw();

    void handlePlayingClick(sf::Vector2f mousePos);
    void endTurn();

    Board m_board;
    static std::vector<sf::RectangleShape> makeObstacles();
    void handle(const sf::Event::Closed& event);
    void handle(const sf::Event::MouseButtonPressed& event);
    void handle(const sf::Event::KeyPressed& event);
    void handle(const auto& event) {};
};
