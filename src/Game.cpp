#include "Game.h"

Game::Game()
    : m_window(sf::VideoMode({ 800u, 600u }), "Parking Mania Prototype"),
    m_obstacles(makeObstacles())
{
	loadTextures();
    m_view.setSize({ 800.f, 600.f });
    m_window.setFramerateLimit(60);
    //try
    m_player = std::make_unique<Player>(m_textures.at("player"));
}

void Game::run()
{
    while (m_window.isOpen())
    {
        handleEvents();

        const float dt = m_clock.restart().asSeconds();
        update(dt);

        draw();
    }
}

void Game::handleEvents()
{
    while (const auto event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            m_window.close();

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
                m_window.close();
        }
    }
}

void Game::update(float dt)
{
    m_player.update(dt);
    // מרכז המצלמה תמיד יהיה על השחקן
    m_view.setCenter(m_player.getPosition());
}

void Game::draw()
{
    m_window.clear(sf::Color(50, 50, 50));

    m_window.setView(m_view);

    for (const auto& obstacle : m_obstacles)
        m_window.draw(obstacle);

    m_player.draw(m_window);
    m_window.display();
}

std::vector<sf::RectangleShape> Game::makeObstacles()
{
    struct ObstacleDesc
    {
        sf::Vector2f size;
        sf::Vector2f pos;
    };

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

void Game::loadTextures()
{
    // ב-SFML 3, loadFromFile מחזיר std::optional<sf::Texture>
    //if (auto tex = sf::Texture::loadFromFile("resources\car_blue.png"))
    //    m_textures["player"] = std::move(*tex);
    sf::Texture tex;
    // 1. קוראים לפונקציה מתוך האובייקט tex שייצרנו
    // 2. שינינו את הלוכסן ל- /
    if (tex.loadFromFile("resources/car_blue.png"))
    {
        // מעבירים את הטקסטורה בצורה יעילה לתוך ה-Map
        m_textures["player"] = std::move(tex);
    }
    /*if (auto tex = sf::Texture::loadFromFile("assets/wall.png"))
        m_textures["wall"] = std::move(*tex);

    if (auto tex = sf::Texture::loadFromFile("assets/roundabout.png"))
        m_textures["roundabout"] = std::move(*tex);*/
}