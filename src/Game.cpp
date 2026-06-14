#include "Game.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({ 800u, 600u }), "Phobies")
    , m_state(GameState::MainMenu)
    , m_obstacles(makeObstacles())
{
    loadTextures();
    m_view.setSize({ 800.f, 600.f });
    m_window.setFramerateLimit(60);

    // Only create player if both textures loaded successfully
    if (m_textures.count("player") && m_textures.count("player_smoke"))
    {
        m_player = std::make_unique<Player>(
            m_textures.at("player"),
            m_textures.at("player_smoke"),
            m_textures.count("player_reverse") ? &m_textures.at("player_reverse") : nullptr);
    }
    else if (m_textures.count("player"))
    {
        // Smoke texture missing — use normal for both (animation just won't flicker)
        m_player = std::make_unique<Player>(
            m_textures.at("player"),
            m_textures.at("player"),
            m_textures.count("player_reverse") ? &m_textures.at("player_reverse") : nullptr);
        std::cerr << "car_blue_smoke.png missing — animation disabled\n";
    }
    else
    {
        std::cerr << "Player NOT created — base texture missing\n";
    }

    try
    {
        m_menu.emplace(m_window);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Menu init failed: " << e.what() << "\n"
                  << "Place a valid TTF file at resources/arial.ttf\n";
        m_window.close();   // run() loop exits immediately
    }
}

// ---------------------------------------------------------------------------
// run() — single loop, dispatches by state
// ---------------------------------------------------------------------------
void Game::run()
{
    if (m_menu.has_value())
        m_menu->reset();

    while (m_window.isOpen())
    {
        handleEvents();

        if (m_state == GameState::Playing)
        {
            const float dt = m_clock.restart().asSeconds();
            update(dt);
        }

        draw();
    }
}

// ---------------------------------------------------------------------------
// handleEvents()
// ---------------------------------------------------------------------------
void Game::handleEvents()
{
    while (const auto event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
            return;
        }

        if (m_state == GameState::MainMenu && m_menu.has_value())
        {
            const auto result = m_menu->handleEvent(*event);
            if (result.has_value())
            {
                if (*result == MenuResult::Play)
                {
                    m_state = GameState::Playing;
                    m_clock.restart();
                }
                else
                {
                    m_window.close();
                }
            }
        }
        else if (m_state == GameState::Playing)
        {
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    m_state = GameState::MainMenu;
                    if (m_menu.has_value())
                        m_menu->reset();
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// update()
// ---------------------------------------------------------------------------
void Game::update(float dt)
{
    if (m_player)
    {
        m_player->update(dt);
        m_view.setCenter(m_player->getPosition());
    }
}

// ---------------------------------------------------------------------------
// draw()
// ---------------------------------------------------------------------------
void Game::draw()
{
    if (m_state == GameState::MainMenu && m_menu.has_value())
    {
        m_window.clear(sf::Color(30, 30, 50));
        m_menu->draw();
    }
    else
    {
        m_window.clear(sf::Color(50, 50, 50));
        m_window.setView(m_view);
        for (const auto& obstacle : m_obstacles)
            m_window.draw(obstacle);
        if (m_player)
            m_player->draw(m_window);
    }

    m_window.display();
}

// ---------------------------------------------------------------------------
// makeObstacles()
// ---------------------------------------------------------------------------
std::vector<sf::RectangleShape> Game::makeObstacles()
{
    struct ObstacleDesc { sf::Vector2f size; sf::Vector2f pos; };

    constexpr ObstacleDesc descs[] = {
        { {120.f,  40.f}, {150.f, 120.f} },
        { { 40.f, 140.f}, {600.f, 200.f} },
        { {180.f,  40.f}, {300.f, 460.f} },
        { { 60.f,  80.f}, { 80.f, 380.f} }
    };

    std::vector<sf::RectangleShape> obstacles;
    obstacles.reserve(std::size(descs));
    for (const auto& desc : descs)
    {
        sf::RectangleShape rect(desc.size);
        rect.setPosition(desc.pos);
        rect.setFillColor(sf::Color(130, 130, 130));
        obstacles.push_back(rect);
    }
    return obstacles;
}

// ---------------------------------------------------------------------------
// loadTextures()
// ---------------------------------------------------------------------------
void Game::loadTextures()
{
    {
        sf::Texture tex;
        if (tex.loadFromFile("resources/car_blue.png"))
            m_textures["player"] = std::move(tex);
        else
            std::cerr << "car_blue.png FAILED to load\n";
    }
    {
        sf::Texture tex;
        if (tex.loadFromFile("resources/car_blue_smoke.png"))
            m_textures["player_smoke"] = std::move(tex);
        else
            std::cerr << "car_blue_smoke.png FAILED to load\n";
    }
    {
        sf::Texture tex;
        if (tex.loadFromFile("resources/car_blue_reverse.png"))
            m_textures["player_reverse"] = std::move(tex);
    }
}
