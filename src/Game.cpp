#include "Game.h"
#include "Constants.h" 
#include <iostream>


Game::Game() : m_board(), m_state(GameState::MainMenu)
{
    m_window.create(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Phobies");
    m_window.setFramerateLimit(60);

    try
    {
        m_menu.emplace(m_window);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Menu init failed: " << e.what() << "\n";
        m_window.close();
    }

    m_player1 = std::make_unique<Player>();
    m_player2 = std::make_unique<Player>();
    m_currentPlayer = m_player1.get(); // שחקן 1 מתחיל

}

void Game::run()
{
    while (m_window.isOpen())
    {
        m_window.clear(sf::Color(30, 30, 50));
        draw();
        m_window.display();

        if (const auto event = m_window.waitEvent())
        {
            event->visit([this](const auto& e) { handle(e); });
        }
        /*if (m_state == GameState::Playing)
        {
            const float dt = m_clock.restart().asSeconds();
            update(dt);
        }*/
    }
}


void Game::handle(const sf::Event::Closed& event)
{
    m_window.close();
}

void Game::handle(const sf::Event::MouseButtonPressed& event)
{
    if (event.button != sf::Mouse::Button::Left)
        return;

    sf::Vector2f pos = m_window.mapPixelToCoords(event.position);

    if (m_state == GameState::Playing)
    {
        // האם הלחיצה היא באזור הקלפים למטה?
        if (pos.y > Config::BOTTOM_PANEL_Y)
        {
            if (m_currentPlayer)
            {
                bool isPlayer2 = (m_currentPlayer == m_player2.get());

                // ננסה לבחור מפלצת מהיד
                auto clickedMonster = m_currentPlayer->handleHandClick(pos, isPlayer2);
                if (clickedMonster)
                {
                    m_selectedFromHand = clickedMonster;
                    // אפשר פה לשים פרינט (cout) כדי לראות שזה עובד: "Monster selected!"
                }
            }
        }
        // הלחיצה היא למעלה באזור הלוח
        else
        {
            // אם יש מפלצת שמחכה ביד לזימון
            if (m_selectedFromHand)
            {
                bool success = m_board.trySpawnMonster(pos, m_selectedFromHand);
                if (success)
                {
                    // הזימון הצליח! נאפס את הבחירה
                    m_selectedFromHand = nullptr;

                    // (בעתיד נוסיף כאן גם: m_currentPlayer->deductKeys(cost))
                }
            }
            // אם לא נבחרה מפלצת מהיד, אז זו סתם לחיצה רגילה על הלוח (הזזה/תקיפה)
            else
            {
                m_board.handleClick(pos);
            }
        }
    }
    // אם אנחנו בתפריט הראשי
    else if (m_state == GameState::MainMenu)
    {
        // כאן נטפל בלחיצות של התפריט (למשל כפתור "Play")
        auto result = m_menu->handleEvent(event);
        if (result.has_value())
        {
            if (*result == MenuResult::Play)
                m_state = GameState::Playing;
            else if (*result == MenuResult::Quit)
                m_window.close();
        }
        return;
    }
}

void Game::handle(const sf::Event::KeyPressed& event)
{
    if (event.code == sf::Keyboard::Key::Escape)
    {
        m_state = GameState::MainMenu;
        if (m_menu.has_value())
            m_menu->reset();
        return;
    }
}

void Game::draw()
{
    if (m_state == GameState::MainMenu && m_menu.has_value())
    {
        m_menu->draw();
    }
    else if (m_state == GameState::Playing)
    {
        m_board.draw(m_window);
        if (m_currentPlayer)
        {
            // אם השחקן הנוכחי הוא שחקן 2 -> נעביר true (יישור לימין)
            // אחרת נעביר false (יישור לשמאל)
            bool isPlayer2 = (m_currentPlayer == m_player2.get());

            m_currentPlayer->drawHand(m_window, isPlayer2);
        }
    }
    
    //m_player1->draw(m_window);
    //m_player2->draw(m_window);
}

//    // Only create player if both textures loaded successfully
//    if (m_textures.count("player") && m_textures.count("player_smoke"))
//    {
//        m_player = std::make_unique<Player>(
//            m_textures.at("player"),
//            m_textures.at("player_smoke"),
//            m_textures.count("player_reverse") ? &m_textures.at("player_reverse") : nullptr);
//    }
//    else if (m_textures.count("player"))
//    {
//        // Smoke texture missing — use normal for both (animation just won't flicker)
//        m_player = std::make_unique<Player>(
//            m_textures.at("player"),
//            m_textures.at("player"),
//            m_textures.count("player_reverse") ? &m_textures.at("player_reverse") : nullptr);
//        std::cerr << "car_blue_smoke.png missing — animation disabled\n";
//    }
//    else
//    {
//        std::cerr << "Player NOT created — base texture missing\n";
//    }
//
//    try
//    {
//        m_menu.emplace(m_window);
//    }
//    catch (const std::exception& e)
//    {
//        std::cerr << "Menu init failed: " << e.what() << "\n"
//                  << "Place a valid TTF file at resources/arial.ttf\n";
//        m_window.close();   // run() loop exits immediately
//    }
//}
//
//// ---------------------------------------------------------------------------
//// run() — single loop, dispatches by state
//// ---------------------------------------------------------------------------
//void Game::run()
//{
//    if (m_menu.has_value())
//        m_menu->reset();
//
//    while (m_window.isOpen())
//    {
//        handleEvents();
//
//        if (m_state == GameState::Playing)
//        {
//            const float dt = m_clock.restart().asSeconds();
//            update(dt);
//        }
//
//        draw();
//    }
//}
//
//// ---------------------------------------------------------------------------
//// handleEvents()
//// ---------------------------------------------------------------------------
//void Game::handleEvents()
//{
//    while (const auto event = m_window.pollEvent())
//    {
//        if (event->is<sf::Event::Closed>())
//        {
//            m_window.close();
//            return;
//        }
//
//        if (m_state == GameState::MainMenu && m_menu.has_value())
//        {
//            const auto result = m_menu->handleEvent(*event);
//            if (result.has_value())
//            {
//                if (*result == MenuResult::Play)
//                {
//                    m_state = GameState::Playing;
//                    m_clock.restart();
//                }
//                else
//                {
//                    m_window.close();
//                }
//            }
//        }
//        else if (m_state == GameState::Playing)
//        {
//            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
//            {
//                if (key->code == sf::Keyboard::Key::Escape)
//                {
//                    m_state = GameState::MainMenu;
//                    if (m_menu.has_value())
//                        m_menu->reset();
//                }
//            }
//        }
//    }
//}
//
//// ---------------------------------------------------------------------------
//// update()
//// ---------------------------------------------------------------------------
//void Game::update(float dt)
//{
//    if (m_player)
//    {
//        m_player->update(dt);
//        m_view.setCenter(m_player->getPosition());
//    }
//}
//
//// ---------------------------------------------------------------------------
//// draw()
//// ---------------------------------------------------------------------------
//void Game::draw()
//{
//    if (m_state == GameState::MainMenu && m_menu.has_value())
//    {
//        m_window.clear(sf::Color(30, 30, 50));
//        m_menu->draw();
//    }
//    else
//    {
//        m_window.clear(sf::Color(50, 50, 50));
//        m_window.setView(m_view);
//        for (const auto& obstacle : m_obstacles)
//            m_window.draw(obstacle);
//        if (m_player)
//            m_player->draw(m_window);
//    }
//
//    m_window.display();
//}
//
//// ---------------------------------------------------------------------------
//// makeObstacles()
//// ---------------------------------------------------------------------------
//std::vector<sf::RectangleShape> Game::makeObstacles()
//{
//    struct ObstacleDesc { sf::Vector2f size; sf::Vector2f pos; };
//
//    constexpr ObstacleDesc descs[] = {
//        { {120.f,  40.f}, {150.f, 120.f} },
//        { { 40.f, 140.f}, {600.f, 200.f} },
//        { {180.f,  40.f}, {300.f, 460.f} },
//        { { 60.f,  80.f}, { 80.f, 380.f} }
//    };
//
//    std::vector<sf::RectangleShape> obstacles;
//    obstacles.reserve(std::size(descs));
//    for (const auto& desc : descs)
//    {
//        sf::RectangleShape rect(desc.size);
//        rect.setPosition(desc.pos);
//        rect.setFillColor(sf::Color(130, 130, 130));
//        obstacles.push_back(rect);
//    }
//    return obstacles;
//}
//
//// ---------------------------------------------------------------------------
//// loadTextures()
//// ---------------------------------------------------------------------------
//void Game::loadTextures()
//{
//    {
//        sf::Texture tex;
//        if (tex.loadFromFile("resources/car_blue.png"))
//            m_textures["player"] = std::move(tex);
//        else
//            std::cerr << "car_blue.png FAILED to load\n";
//    }
//    {
//        sf::Texture tex;
//        if (tex.loadFromFile("resources/car_blue_smoke.png"))
//            m_textures["player_smoke"] = std::move(tex);
//        else
//            std::cerr << "car_blue_smoke.png FAILED to load\n";
//    }
//    {
//        sf::Texture tex;
//        if (tex.loadFromFile("resources/car_blue_reverse.png"))
//            m_textures["player_reverse"] = std::move(tex);
//    }
//}
