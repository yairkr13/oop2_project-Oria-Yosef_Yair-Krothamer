#include "Board.h"

Board::Board() 
{
    createBoard();
}


//void Board::setMonsters(std::vector<Monster*> monsters)
//{
//    m_monsters = std::move(monsters);
//}

void Board::createBoard()
{
    float width = std::sqrt(3.f) * TILE_RADIUS;

    int max_rows = 7;
    int max_cols = 14;

    for (int row = 0; row < max_rows; ++row)
    {
        int start_col = (row % 2 == 0) ? 0 : 1;

        for (int q = start_col; q < max_cols; q += 2)
        {
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * TILE_RADIUS) * row;

            sf::Vector2f physicalPosition(x, y);
            m_grid[{q, row}] = std::make_unique<Tile>(q, row, physicalPosition);
        }
    }
}

void Board::draw(sf::RenderWindow& window) const
{
    // 1. מציירים את כל משושי הרקע קודם
    for (auto const& [coords, tile] : m_grid)
    {
        tile->draw(window);
    }

    // 2. עכשיו מציירים את המפלצות שעומדות על המשושים
    for (auto const& [coords, tile] : m_grid)
    {
        // lock() הופך את weak_ptr ל-shared_ptr זמני. אם יש מפלצת - נצייר אותה
        if (auto monster = tile->getMonster())
        {
            sf::Vector2f tilePos = tileToScreen(tile->getQ(), tile->getRow());
            monster->setScreenPosition(tilePos);
            monster->draw(window);
        }
    }
}

void Board::handleClick(const sf::Vector2f& pos)
{
    // 1. קודם כל, נבדוק על איזה משושה (Tile) השחקן לחץ בכלל
    Tile* clickedTile = nullptr;
    for (auto& [coords, tile] : m_grid)
    {
        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
        float dx = pos.x - tileCenter.x;
        float dy = pos.y - tileCenter.y;

        // משפט פיתגורס לבדיקה האם הלחיצה בתוך הרדיוס של המשושה
        if ((dx * dx + dy * dy) <= (TILE_RADIUS * TILE_RADIUS))
        {
            clickedTile = tile.get();
            break;
        }
    }

    // אם לחצו מחוץ ללוח, אין לנו מה לעשות
    if (!clickedTile) return;

    // 2. נחפש אם כבר יש מפלצת שנבחרה קודם לכן בלוח
    std::shared_ptr<Monster> selectedMonster = nullptr;
    Tile* selectedMonsterTile = nullptr;

    for (auto& [coords, tile] : m_grid)
    {
        if (auto monster = tile->getMonster())
        {
            //sf::Vector2f tilePos = tileToScreen(clickedTile->getQ(), clickedTile->getRow());
            //selectedMonster->walkTo(posOnScreen);
            // 
            //monster->setScreenPosition(tilePos); // <--- הנה השורת קסם שחסרה!
            //monster->draw(window);
            if (monster->isSelected())
            {
                selectedMonster = monster;
                selectedMonsterTile = tile.get();
                break;
            }
        }
    }

    // 3. הלוגיקה של הלחיצה:
    if (selectedMonster)
    {
        if (clickedTile->isHighlighted() && !clickedTile->hasMonster())
        {
            clickedTile->setMonster(selectedMonster);     // שמים את המפלצת במשבצת החדשה
            selectedMonsterTile->setMonster(nullptr);     // מוחקים אותה מהמשבצת הישנה

            // ברגע ששינינו את ה-Q וה-Row, הפונקציה draw כבר "תשגר" אותה לשם אוטומטית!
            selectedMonster->setPosition(clickedTile->getQ(), clickedTile->getRow());
        }
        /*if(clickedTile->hasMonster())
			selectedMonster->attack(clickedTile->getMonster());*/
        // בכל מקרה, אחרי שלחצנו (לזוז או לבטל), מורידים את הסימון
        selectedMonster->setSelected(false);
        clearHighlights();
    }
    else
    {
        // אם אף מפלצת לא מסומנת, נבדוק אם לחצנו על משבצת שיש בה מפלצת
        if (auto monster = clickedTile->getMonster())
        {
            monster->setSelected(true);
            highlightNeighbors(clickedTile->getQ(), clickedTile->getRow(), monster->getRange());
        }
    }
}

bool Board::trySpawnMonster(const sf::Vector2f& pos, std::shared_ptr<Monster> monster)
{
    Tile* clickedTile = nullptr;

    // מוצאים על איזה משושה לחצו
    for (auto& [coords, tile] : m_grid)
    {
        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
        float dx = pos.x - tileCenter.x;
        float dy = pos.y - tileCenter.y;
        if ((dx * dx + dy * dy) <= (TILE_RADIUS * TILE_RADIUS))
        {
            clickedTile = tile.get();
            break;
        }
    }

    // אם לחצנו על משבצת חוקית, והיא ריקה (אין עליה כבר מפלצת)
    if (clickedTile && !clickedTile->hasMonster())
    {
        clickedTile->setMonster(monster);
        // מעדכנים רק את המיקום הלוגי בלוח (Q, Row)
        monster->setPosition(clickedTile->getQ(), clickedTile->getRow());
        return true;
    }

    return false; // הזימון נכשל (משבצת תפוסה או לחיצה מחוץ ללוח)
}

// BFS
void Board::highlightNeighbors(int q, int row, int range)
{
    std::pair<int, int> offsets[] = {
        {-2,  0}, {+2,  0},
        {-1, -1}, {+1, -1},
        {-1, +1}, {+1, +1}
    };

    std::map<std::pair<int, int>, int> visited;
    std::vector<std::pair<int, int>> frontier;
    visited[{q, row}] = 0;
    frontier.push_back({ q, row });

    while (!frontier.empty())
    {
        std::vector<std::pair<int, int>> nextFrontier;
        for (auto [cq, cr] : frontier)
        {
            int dist = visited[{cq, cr}];
            if (dist >= range)
                continue;

            for (auto [dq, dr] : offsets)
            {
                std::pair<int, int> neighbor = { cq + dq, cr + dr };
                if (visited.count(neighbor))
                    continue;
                auto it = m_grid.find(neighbor);
                if (it == m_grid.end())
                    continue;

                visited[neighbor] = dist + 1;
                it->second->setHighlighted(true);
                nextFrontier.push_back(neighbor);
            }
        }
        frontier = std::move(nextFrontier);
    }
}

void Board::clearHighlights()
{
    for (auto& pair : m_grid)
    {
        pair.second->setHighlighted(false);
    }
}

sf::Vector2f Board::tileToScreen(int q, int row) const
{
    float width = std::sqrt(3.f) * TILE_RADIUS;

    // עכשיו זה תואם בדיוק ל-createBoard!
    float x = START_X + (width / 2.f) * q;
    float y = START_Y + (1.5f * TILE_RADIUS) * row;

    return { x + TILE_RADIUS, y + TILE_RADIUS };
}
/*
void Board::handleClick(const sf::Vector2f& pos)
{
    // If a monster is selected, check if click is on a highlighted tile
    Monster* selected = nullptr;
    for (auto* monster : m_monsters)
    {
        if (monster->isSelected())
        {
            selected = monster;
            break;
        }
    }

    if (selected)
    {
        for (auto& [coords, tile] : m_grid)
        {
            if (!tile->isHighlighted())
                continue;

            sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
            float dx = pos.x - tileCenter.x;
            float dy = pos.y - tileCenter.y;
            if ((dx * dx + dy * dy) <= (TILE_RADIUS * TILE_RADIUS))
            {
                selected->setPosition(coords.first, coords.second);
                selected->setSelected(false);
                clearHighlights();
                return;
            }
        }

        selected->setSelected(false);
        clearHighlights();
        return;
    }

    // No monster selected — check if clicking a monster to select it
    for (auto* monster : m_monsters)
    {
        sf::Vector2f screenPos = tileToScreen(monster->getQ(), monster->getRow());
        if (monster->contains(pos, screenPos))
        {
            monster->setSelected(true);
            highlightNeighbors(monster->getQ(), monster->getRow(), monster->getRange());
            return;
        }
    }
}

//BFS
void Board::highlightNeighbors(int q, int row, int range)
{
    std::pair<int, int> offsets[] = {
        {-2,  0}, {+2,  0},
        {-1, -1}, {+1, -1},
        {-1, +1}, {+1, +1}
    };

    std::map<std::pair<int, int>, int> visited;
    std::vector<std::pair<int, int>> frontier;
    visited[{q, row}] = 0;
    frontier.push_back({ q, row });

    while (!frontier.empty())
    {
        std::vector<std::pair<int, int>> nextFrontier;
        for (auto [cq, cr] : frontier)
        {
            int dist = visited[{cq, cr}];
            if (dist >= range)
                continue;

            for (auto [dq, dr] : offsets)
            {
                std::pair<int, int> neighbor = { cq + dq, cr + dr };
                if (visited.count(neighbor))
                    continue;
                auto it = m_grid.find(neighbor);
                if (it == m_grid.end())
                    continue;

                visited[neighbor] = dist + 1;
                it->second->setHighlighted(true);
                nextFrontier.push_back(neighbor);
            }
        }
        frontier = std::move(nextFrontier);
    }
}

void Board::clearHighlights()
{
    for (auto& pair : m_grid)
    {
        pair.second->setHighlighted(false);
    }
}

void Board::addMonster(Monster* monster)
{
    m_monsters.push_back(monster);
}*/