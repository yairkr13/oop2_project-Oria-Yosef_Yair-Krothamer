#include "Game.h"
#include "Constants.h"
#include "TextureManager.h"
#include <iostream>

const sf::Texture& Game::initResourcesAndGetBg()
{
    loadAllResources(); // קודם כל טוענים את כל המשחק
    return TextureManager::getInstance().get<sf::Texture>("game_bg"); // ואז מחזירים את הרקע
}

Game::Game() : m_board(), m_state(GameState::MainMenu),
    /*m_bgSprite([]() -> const sf::Texture& {
        TextureManager::getInstance().loadTexture("game_bg", "resources/Background/Background1.png");
        return TextureManager::getInstance().getTexture("game_bg");
    }())*/
	m_bgSprite(initResourcesAndGetBg())
{
    
    m_bgSprite.setTexture(TextureManager::getInstance().get<sf::Texture>("game_bg"));
    m_window.create(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Phobies");
    m_window.setFramerateLimit(60);

    // Scale background to fit window

    auto texSize = m_bgSprite.getTexture().getSize();
    m_bgSprite.setScale({ static_cast<float>(Config::WINDOW_WIDTH) / texSize.x,
                          static_cast<float>(Config::WINDOW_HEIGHT) / texSize.y });

    try
    {
        m_menu.emplace(m_window);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Menu init failed: " << e.what() << "\n";
        m_window.close();
    }

    m_player1 = std::make_unique<Player>(PlayerSide::Left);
    m_player2 = std::make_unique<Player>(PlayerSide::Right);
    m_currentPlayer = m_player1.get();

}

void Game::run()
{
    while (m_window.isOpen())
    {
        while (const auto event = m_window.pollEvent())
        {
            event->visit([this](const auto& e) { handle(e); });
        }

        m_window.clear(sf::Color(30, 30, 50));
        draw();
        m_window.display();
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
                    // Toggle: if clicking the same card, deselect it
                    if (m_selectedFromHand == clickedMonster)
                    {
                        m_selectedFromHand = nullptr;
                        m_board.clearHighlights();
                    }
                    else
                    {
                        m_selectedFromHand = clickedMonster;
                        m_board.highlightSpawnTiles(m_currentPlayer->getSide());
                    }
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
                    m_currentPlayer->reduceKeys(m_selectedFromHand->getCost()); // end turn after successful spawn
                    m_board.clearHighlights();
                    m_selectedFromHand = nullptr;
                }
            }
            // אם לא נבחרה מפלצת מהיד, אז זו סתם לחיצה רגילה על הלוח (הזזה/תקיפה)
            else
            {
                m_board.handleClick(pos, m_currentPlayer->getSide());
            }
        }
    }

    else if (m_state == GameState::MainMenu)
    {
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
    if (m_state == GameState::Playing && event.code == sf::Keyboard::Key::Space)
    {
        // 1. העברת התור לשחקן השני
        if (m_currentPlayer == m_player1.get())
        {
            m_currentPlayer = m_player2.get();
        }
        else
        {
            m_currentPlayer = m_player1.get();
        }

        m_selectedFromHand = nullptr; // ביטול בחירת קלף מהיד
        //m_board.clearHighlights();    // ניקוי משבצות מוארות בלוח
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
        m_window.draw(m_bgSprite);
        m_board.draw(m_window);
        if (m_currentPlayer)
        {
            bool isPlayer2 = (m_currentPlayer == m_player2.get());

            m_currentPlayer->draw(m_window, isPlayer2, m_selectedFromHand);
        }
    }
    
    //m_player1->draw(m_window);
    //m_player2->draw(m_window);
}

void Game::loadAllResources()
{
    auto& rm = TextureManager::getInstance();

    try
    {
        // --- 1. פונטים ---
        rm.load<sf::Font>("arial", "resources/Fonts/arial.ttf");
        rm.load<sf::Font>("Lilita", "resources/Fonts/LilitaOne.ttf");

        // --- 2. ממשק ותפריטים ---
        rm.load<sf::Texture>("menu_bg", "resources/Menu/Menu.png");
        rm.load<sf::Texture>("instructions_bg", "resources/Menu/Instructions.png");
        rm.load<sf::Texture>("game_bg", "resources/Background/Background1.png");
        //rm.load<sf::Texture>("card_bg", "resources/Cards/card_bg.png");

        // --- 3. מפלצות (Phobies) ---
        // שים לב: אנחנו טוענים רק את תמונת הבסיס (למשל ימין) והמחלקה Monster תהפוך אותה אוטומטית לשמאל כשצריך!

        // Muffintop
        rm.load<sf::Texture>("muffintop", "resources/Monster/Muffintop/Muffintop_R.png");
        rm.load<sf::Texture>("muffintop_card", "resources/Monster/Muffintop/Muffintop_card_R.png");

        // Blue
        rm.load<sf::Texture>("blue", "resources/Monster/Blue/Blue_R.png");
        rm.load<sf::Texture>("blue_card", "resources/Monster/Blue/Blue_card_R.png");

        // Barzilla
        rm.load<sf::Texture>("barzilla", "resources/Monster/Barzilla/Barzilla_R.png");
        rm.load<sf::Texture>("barzilla_card", "resources/Monster/Barzilla/Barzilla_card_R.png");

        // Henrietta
        rm.load<sf::Texture>("henrietta", "resources/Monster/Henrietta/Henrietta_R.png");
        rm.load<sf::Texture>("henrietta_card", "resources/Monster/Henrietta/Henrietta_card_R.png");

        // Mozzy
        rm.load<sf::Texture>("mozzy", "resources/Monster/Mozzy/Mozzy_R.png");
        rm.load<sf::Texture>("mozzy_card", "resources/Monster/Mozzy/Mozzy_card_R.png");
    }
    catch (const std::exception& e)
    {
        std::cerr << "CRITICAL ERROR during resource loading: " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
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
