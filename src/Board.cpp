#include "Board.h"
#include "LavaTile.h"
#include "Hole.h"
#include "PanicPoint.h"
#include <random>

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

    // וקטור זמני שישמור את כל הקורדינטות החוקיות שנוצרו
    //std::vector<std::pair<int, int>> allCoords;

    // שלב א': יצירת הלוח כרגיל עם משבצות רגילות
    for (int row = 0; row < max_rows; ++row)
    {
        int start_col = (row % 2 == 0) ? 0 : 1;

        for (int q = start_col; q < max_cols; q += 2)
        {
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * TILE_RADIUS) * row;

            sf::Vector2f physicalPosition(x, y);
            m_grid[{q, row}] = std::make_unique<Tile>(q, row, physicalPosition);
/*
            if(q!=1 && q!=12)
                allCoords.push_back({ q, row }); */// שומרים את הקורדינטה שנוצרה
        }


    }

    // שלב ב': בחירה רנדומלית של 2 מיקומים והחלפתם בלבה
    //if (allCoords.size() >= 4)
    //{
    //    std::random_device rd;
    //    std::mt19937 g(rd());
    //    std::shuffle(allCoords.begin(), allCoords.end(), g); // מערבב את המיקומים החוקיים

    //    // 2 המיקומים הראשונים יהיו לבה
    //    for (int i = 0; i < 2; ++i)
    //    {
    //        auto [q, row] = allCoords[i];
    //        float x = START_X + (width / 2.f) * q;
    //        float y = START_Y + (1.5f * TILE_RADIUS) * row;
    //        m_grid[{q, row}] = std::make_unique<LavaTile>(q, row, sf::Vector2f(x, y));
    //    }
    //    //ליצור פונקציה שפשוט מביאה מישהו מהאקראים !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //    // 2 המיקומים הבאים (2 ו-3) יהיו חורים
    //    for (int i = 2; i < 4; ++i)
    //    {
    //        auto [q, row] = allCoords[i];
    //        float x = START_X + (width / 2.f) * q;
    //        float y = START_Y + (1.5f * TILE_RADIUS) * row;
    //        m_grid[{q, row}] = std::make_unique<Hole>(q, row, sf::Vector2f(x, y));
    //    }
    //}
}
//void Board::createBoard()
//{
//    float width = std::sqrt(3.f) * TILE_RADIUS;
//
//    int max_rows = 7;
//    int max_cols = 14;
//
//    for (int row = 0; row < max_rows; ++row)
//    {
//        int start_col = (row % 2 == 0) ? 0 : 1;
//
//        for (int q = start_col; q < max_cols; q += 2)
//        {
//            float x = START_X + (width / 2.f) * q;
//            float y = START_Y + (1.5f * TILE_RADIUS) * row;
//
//            sf::Vector2f physicalPosition(x, y);
//            m_grid[{q, row}] = std::make_unique<Tile>(q, row, physicalPosition);
//        }
//    }
//}

void Board::draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const
{
    // 1. ������� �� �� ����� ���� ����
    for (auto const& [coords, tile] : m_grid)
    {
        tile->draw(window, currentTurnSide);
       /* if (tile->hasHeart()) {
            tile->getHeart()->draw(window);
        }*/
    }

    // 2. ����� ������� �� ������� ������� �� �������
    //for (auto const& [coords, tile] : m_grid)
    //{
    //    // lock() ���� �� weak_ptr �-shared_ptr ����. �� �� ����� - ����� ����
    //    if (auto monster = tile->getMonster())
    //    {
    //        monster->draw(window, currentTurnSide);
    //    }
    //}
}

// בתוך Board.cpp
// בתוך Board.cpp
void Board::initPlayerHearts(Heart* p1Heart, Heart* p2Heart) {
    int middleRow = 3; // השורה האמצעית של הלוח

    // 1. מיקום הלב של שחקן 1 (הכי שמאלי)
    Tile* p1Tile = getLeftmostTileInRow(middleRow);
    if (p1Tile != nullptr && p1Heart != nullptr) {
        sf::Vector2f screenPos = tileToScreen(p1Tile->getQ(), p1Tile->getRow());
        p1Heart->spawnOnBoard(p1Tile->getQ(), p1Tile->getRow(), screenPos); // הפונקציה הפשוטה מההודעה הקודמת
        p1Tile->setEntity(p1Heart);
    }

    // 2. מיקום הלב של שחקן 2 (הכי ימני)
    Tile* p2Tile = getRightmostTileInRow(middleRow);
    if (p2Tile != nullptr && p2Heart != nullptr) {
        sf::Vector2f screenPos = tileToScreen(p2Tile->getQ(), p2Tile->getRow());
        p2Heart->spawnOnBoard(p2Tile->getQ(), p2Tile->getRow(), screenPos);
        p2Tile->setEntity(p2Heart);
    }
    generateSpecialTiles(p1Heart, p2Heart);
}
void Board::generateSpecialTiles(Heart* p1Heart, Heart* p2Heart)
{
    float width = std::sqrt(3.f) * TILE_RADIUS;

    std::vector<std::pair<int, int>> allCoords;

    // אוספים משבצות רק מהאיזור המותר (בלי הטורים הקיצוניים של הזימונים והלבבות)
    for (auto const& [coords, tile] : m_grid)
    {
        int q = coords.first;
        if (q >= 2 && q <= 11)
        {
            allCoords.push_back(coords);
        }
    }

    if (allCoords.size() >= 5)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allCoords.begin(), allCoords.end(), g);

        // 2 משבצות לבה
        for (int i = 0; i < 2; ++i) {
            auto [q, row] = allCoords[i];
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * TILE_RADIUS) * row;
            m_grid[{q, row}] = std::make_unique<LavaTile>(q, row, sf::Vector2f(x, y));
        }

        // 2 משבצות חור
        for (int i = 2; i < 4; ++i) {
            auto [q, row] = allCoords[i];
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * TILE_RADIUS) * row;
            m_grid[{q, row}] = std::make_unique<Hole>(q, row, sf::Vector2f(x, y));
        }

        // 1 משבצת פאניקה שמקבלת את הלבבות שהוזרקו לפונקציה!
        for (int i = 4; i < 5; ++i) {
            auto [q, row] = allCoords[i];
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * TILE_RADIUS) * row;
            m_grid[{q, row}] = std::make_unique<PanicPoint>(q, row, sf::Vector2f(x, y), p1Heart, p2Heart);
        }
    }
}

void Board::handleClick(const sf::Vector2f& pos, PlayerSide currentSide)
{
    // 1. מציאת המשבצת עליה לחצו
    Tile* clickedTile = nullptr;
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

    if (!clickedTile) return;

    // 2. מציאת המפלצת שנבחרה קודם לכן
    Monster* selectedMonster = nullptr;
    for (auto& [coords, tile] : m_grid)
    {
        if (auto entity = tile->getEntity()) // שלב א: לוקחים את הישות הכללית
        {
            // שלב ב: הלוח שואל "האם אתה מפלצת?" 
            if (entity->getType() == EntityType::Monster)
            {
                // עכשיו אנחנו בטוחים שזו מפלצת, אז אפשר להמיר בבטחה
                Monster* monster = static_cast<Monster*>(entity);

                if (monster->isSelected())
                {
                    selectedMonster = monster;
                    break;
                }
            }
        }
        //if (auto monster = tile->getMonster())
        //{
        //    /*Monster* monster;
        //    if (entity->isSelectable())
        //        monster = static_cast<Monster*>(entity);
        //    else
        //        return;*/
        //    if (monster->isSelected())
        //    {
        //        selectedMonster = monster;
        //        break;
        //    }
        //}
    }

    // 3. ביצוע הפעולה בעזרת פונקציית הליבה המשותפת!
    if (selectedMonster)
    {
        // אם לחצנו על משבצת חוקית (מוארת) - פונקציית הליבה כבר תדע אם לזוז או לתקוף
        if (clickedTile->isHighlighted())
        {
            performAction(selectedMonster, clickedTile);
        }

        // ניקוי וביטול בחירה
        selectedMonster->setSelected(false);
        clearHighlights();
    }
    else
    {
        // 4. בחירת מפלצת חדשה
        if (auto entity = clickedTile->getEntity())
        {
            // בודקים אם הישות היא מפלצת
            if (entity->getType() == EntityType::Monster)
            {
                Monster* monster = static_cast<Monster*>(entity);

                if (monster->getSide() != currentSide || monster->getActionsLeft() <= 0) return;

                monster->setSelected(true);
                highlightNeighbors(monster);
            }
        }
        //if (auto monster = clickedTile->getMonster())
        //{
        //    /*Monster* monster;
        //    if (entity->isSelectable())
        //        monster = static_cast<Monster*>(entity);
        //    else
        //        return;*/
        //    if (monster->getSide() != currentSide || monster->getActionsLeft() <= 0) return;

        //    monster->setSelected(true);
        //    highlightNeighbors(monster);
        //}
    }
}
//void Board::handleClick(const sf::Vector2f& pos, PlayerSide currentSide)
//{
//    // 1. מציאת המשבצת עליה לחצו
//    Tile* clickedTile = nullptr;
//    for (auto& [coords, tile] : m_grid)
//    {
//        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
//        float dx = pos.x - tileCenter.x;
//        float dy = pos.y - tileCenter.y;
//
//        if ((dx * dx + dy * dy) <= (TILE_RADIUS * TILE_RADIUS))
//        {
//            clickedTile = tile.get();
//            break;
//        }
//    }
//
//    if (!clickedTile) return;
//
//    // 2. מציאת המפלצת שנבחרה קודם לכן
//    Monster* selectedMonster = nullptr;
//    Tile* selectedMonsterTile = nullptr;
//
//    for (auto& [coords, tile] : m_grid)
//    {
//        if (auto monster = tile->getMonster())
//        {
//            if (monster->isSelected())
//            {
//                selectedMonster = monster;
//                selectedMonsterTile = tile.get();
//                break;
//            }
//        }
//    }
//
//    // 3. ביצוע הפעולה בצורה פולימורפית ונקייה
//    if (selectedMonster)
//    {
//        // א. תנועה למשבצת ריקה ומוארת
//        if (clickedTile->isHighlighted() && !clickedTile->hasEntity())
//        {
//            clickedTile->setMonster(selectedMonster);
//            selectedMonsterTile->clearEntity();
//
//            // שימוש בפונקציה החדשה - המפלצת תוריד לעצמה את הפעולה!
//            selectedMonster->moveTo(clickedTile->getQ(), clickedTile->getRow(),
//                tileToScreen(clickedTile->getQ(), clickedTile->getRow()));
//        }
//
//        // ב. התקפה על ישות אויב
//        if (clickedTile->hasEntity())
//        {
//            auto target = clickedTile->getEntity();
//
//            if (target->getSide() != currentSide && clickedTile->isHighlighted())
//            {
//                // המפלצת תתקוף ותוריד לעצמה את הפעולה בפנים!
//                selectedMonster->attack(target);
//
//                if (!target->isAlive())
//                    clickedTile->clearEntity();
//            }
//        }
//
//        // ניקוי וביטול בחירה
//        selectedMonster->setSelected(false);
//        clearHighlights();
//    }
//    else
//    {
//        // 4. בחירת מפלצת חדשה
//        if (auto monster = clickedTile->getMonster())
//        {
//            if (monster->getSide() != currentSide) return;
//
//            // הלוח רק בודק: אם אין לה פעולות, הוא פשוט מסרב לבחור בה!
//            if (monster->getActionsLeft() <= 0) return;
//
//            monster->setSelected(true);
//            highlightNeighbors(monster);
//        }
//    }
//}
//void Board::handleClick(const sf::Vector2f& pos, PlayerSide currentSide)
//{
//    // 1. ���� ��, ����� �� ���� ����� (Tile) ����� ��� ����
//    Tile* clickedTile = nullptr;
//    for (auto& [coords, tile] : m_grid)
//    {
//        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
//        float dx = pos.x - tileCenter.x;
//        float dy = pos.y - tileCenter.y;
//
//        // ���� ������� ������ ��� ������ ���� ������ �� ������
//        if ((dx * dx + dy * dy) <= (TILE_RADIUS * TILE_RADIUS))
//        {
//            clickedTile = tile.get();
//            break;
//        }
//    }
//
//    // �� ���� ���� ����, ��� ��� �� �����
//    if (!clickedTile) return;
//
//    // 2. ���� �� ��� �� ����� ������ ���� ��� ����
//    //std::shared_ptr<Monster> selectedMonster = nullptr;
//	Monster* selectedMonster = nullptr;
//    Tile* selectedMonsterTile = nullptr;
//
//    for (auto& [coords, tile] : m_grid)
//    {
//        if (auto monster = tile->getMonster())
//        {
//            
//            //sf::Vector2f tilePos = tileToScreen(clickedTile->getQ(), clickedTile->getRow());
//            //selectedMonster->walkTo(posOnScreen);
//            // 
//            //monster->setScreenPosition(tilePos); // <--- ��� ����� ��� �����!
//            //monster->draw(window);
//            if (monster->isSelected())
//            {
//                selectedMonster = monster;
//                selectedMonsterTile = tile.get();
//                break;
//            }
//        }
//    }
//
//    // 3. ������� �� ������:
//    if (selectedMonster)
//    {
//        if (clickedTile->isHighlighted() && !clickedTile->hasEntity())
//        {
//            clickedTile->setMonster(selectedMonster);     // ���� �� ������ ������ �����
//            //selectedMonsterTile->setMonster(nullptr);     // ������ ���� ������� �����
//            selectedMonsterTile->clearEntity(); // <--- התיקון כאן: שימוש בפונקציית הניקוי החדשה
//            // ���� ������� �� �-Q ��-Row, �������� draw ��� "����" ���� ��� ��������!
//            //selectedMonster->setPosition(clickedTile->getQ(), clickedTile->getRow());
//			selectedMonster->spawnOnBoard(clickedTile->getQ(), clickedTile->getRow(), 
//                tileToScreen(clickedTile->getQ(), clickedTile->getRow()));
//        }
//        if (clickedTile->hasEntity())
//        {
//            auto target = clickedTile->getEntity();
//
//            // בדיקה: האם המטרה היא אויב והאם המשבצת בטווח התקפה/תנועה (מוארת)?
//            if (target->getSide() != currentSide && clickedTile->isHighlighted())
//            {
//                selectedMonster->attack(target);
//                if (!target->isAlive())
//					clickedTile->clearEntity(); // <--- התיקון כאן: שימוש בפונקציית הניקוי החדשה
//                    //clickedTile->setMonster(nullptr);
//
//            }
//        }
//        // ��� ����, ���� ������ (���� �� ����), ������� �� ������
//        selectedMonster->setSelected(false);
//        clearHighlights();
//    }
//    else
//    {
//        // �� �� ����� �� ������, ����� �� ����� �� ����� ��� �� �����
//        if (auto monster = clickedTile->getMonster())
//        {
//            // Only allow selecting your own monsters
//            if (monster->getSide() != currentSide) return;
//
//            monster->setSelected(true);
//            highlightNeighbors(monster);
//            //highlightNeighbors(clickedTile->getQ(), clickedTile->getRow(), monster->getRange());
//        }
//    }
//}


bool Board::trySpawnMonster(const sf::Vector2f& pos, Monster* monster)
{
    if (!monster) return false;

    for (auto& [coords, tile] : m_grid)
    {
        if (tile->isHighlighted() &&
            std::hypot(pos.x - tileToScreen(coords.first, coords.second).x,
                pos.y - tileToScreen(coords.first, coords.second).y) < TILE_RADIUS)
        {
            // משתמשים בפונקציית הליבה לזימון!
            if (spawnMonsterOnTile(monster, tile.get()))
            {
                clearHighlights();
                return true;
            }
        }
    }
    return false;
}
//returns true if the monster was successfully spawned on the board, false otherwise
//bool Board::trySpawnMonster(const sf::Vector2f& pos, Monster* monster)
//{
//    if (!monster) return false; // תמיד טוב לבדוק שהמצביע חוקי
//
//    for (auto& [coords, tile] : m_grid)
//    {
//        if (tile->isHighlighted() &&
//            std::hypot(pos.x - tileToScreen(coords.first, coords.second).x,
//                pos.y - tileToScreen(coords.first, coords.second).y) < TILE_RADIUS)
//        {
//            if (tile->isPassable())
//            {
//                // המשבצת שומרת את המצביע הרגיל לישות
//                tile->setMonster(monster);
//
//                // המפלצת מתעדכנת על המיקום החדש שלה
//                //monster->setBoardPosition(coords.first, coords.second, tileToScreen(coords.first, coords.second));
//                monster->spawnOnBoard(coords.first, coords.second, tileToScreen(coords.first, coords.second));
//
//                clearHighlights();
//                return true;
//            }
//        }
//    }
//    return false;
//}
//bool Board::trySpawnMonster(const sf::Vector2f& pos, std::shared_ptr<Monster> monster)
//{
//    Tile* clickedTile = nullptr;
//
//    for (auto& [coords, tile] : m_grid)
//    {
//        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
//        float dx = pos.x - tileCenter.x;
//        float dy = pos.y - tileCenter.y;
//        if ((dx * dx + dy * dy) <= (TILE_RADIUS * TILE_RADIUS))
//        {
//            clickedTile = tile.get();
//            break;
//        }
//    }
//
//    // Only allow spawning on highlighted (valid) tiles that are empty
//    if (clickedTile && clickedTile->isHighlighted() && !clickedTile->hasMonster())
//    {
//        clickedTile->setMonster(monster);
//        monster->spawnOnBoard(clickedTile->getQ(), clickedTile->getRow(), 
//			tileToScreen(clickedTile->getQ(), clickedTile->getRow()));
//        //monster->setPosition(clickedTile->getQ(), clickedTile->getRow());
//        return true;
//    }
//
//    return false; 
//}
//
void Board::highlightSpawnTiles(PlayerSide side)
{
    clearHighlights();
    for (auto& [coords, tile] : m_grid)
    {
        if (tile->hasEntity()) continue;

        if (side == PlayerSide::Left && coords.first <= 1)
        {
            tile->setHighlighted(true);
        }
        else if (side == PlayerSide::Right && coords.first >= 12)
        {
            tile->setHighlighted(true);
        }
    }
}

// BFS
//void Board::highlightNeighbors(int q, int row, int range)
//{
//    std::pair<int, int> offsets[] = {
//        {-2,  0}, {+2,  0},
//        {-1, -1}, {+1, -1},
//        {-1, +1}, {+1, +1}
//    };
//
//    std::map<std::pair<int, int>, int> visited;
//    std::vector<std::pair<int, int>> frontier;
//    visited[{q, row}] = 0;
//    frontier.push_back({ q, row });
//
//    while (!frontier.empty())
//    {
//        std::vector<std::pair<int, int>> nextFrontier;
//        for (auto [cq, cr] : frontier)
//        {
//            int dist = visited[{cq, cr}];
//            if (dist >= range)
//                continue;
//
//            for (auto [dq, dr] : offsets)
//            {
//                std::pair<int, int> neighbor = { cq + dq, cr + dr };
//                if (visited.count(neighbor))
//                    continue;
//                auto it = m_grid.find(neighbor);
//                if (it == m_grid.end())
//                    continue;
//
//                visited[neighbor] = dist + 1;
//                it->second->setHighlighted(true);
//                nextFrontier.push_back(neighbor);
//            }
//        }
//        frontier = std::move(nextFrontier);
//    }
//}
void Board::highlightNeighbors(Monster* monster)
{
    if (!monster) return;

    int q = monster->getQ();
    int row = monster->getRow();
    int range = monster->getRange();

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

                // --- שדרוג 1: בדיקת חור ותעופה ---
                if (it->second->isHole() && !monster->canFly())
                {
                    // מפלצת קרקע לא יכולה ללכת על בור, אבל היא כן יכולה לתקוף אויב שעף מעליו!
                    if (it->second->hasEntity() && it->second->getEntity()->getSide() != monster->getSide())
                    {
                        // מצאנו אויב מעופף מעל הבור - נצבע אותו באדום כדי לאפשר תקיפה
                        it->second->setHighlighted(true, sf::Color(255, 90, 90, 180));
                    }

                    // בכל מקרה, מפלצת קרקע לא יכולה לעבור דרך החור או לעמוד עליו, אז נעצור את התנועה פה
                    continue;
                }
                visited[neighbor] = dist + 1;

                // --- שדרוג 2: זיהוי אויבים ולבבות וצביעה באדום ---
                if (it->second->hasEntity() && it->second->getEntity()->getSide() != monster->getSide())
                {
                    // מצאנו ישות (מפלצת או לב) ששייכת לאויב! נצבע באדום חצי שקוף
                    it->second->setHighlighted(true, sf::Color(255, 90, 90, 180));

                    // חוק Phobies חשוב: מפלצת קרקע לא יכולה ללכת *מעבר* לאויב, האויב חוסם את המשך הדרך
                    if (!monster->canFly()) continue;
                }
                else
                {
                    // משבצת ריקה, לבה או בעלת ברית - נצבע בירוק רגיל (ברירת המחדל של הפונקציה)
                    it->second->setHighlighted(true);
                }

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

    // ����� �� ���� ����� �-createBoard!
    float x = START_X + (width / 2.f) * q;
    float y = START_Y + (1.5f * TILE_RADIUS) * row;

    return { x + TILE_RADIUS, y + TILE_RADIUS };
}

void Board::updateTileEffects()
{
    for (auto& [coords, tile] : m_grid)
    {
        tile->applyTileEffect(); // פולימורפיזם בפעולה! משבצת רגילה לא תעשה כלום, לבה תוריד חיים.

        // אם המפלצת מתה מהאפקט (למשל מהלבה), ננקה אותה מהמשבצת
        if (tile->hasEntity())
        {
            if (auto monster = tile->getEntity())
            {
                if (!monster->isAlive()) // משתמש במתודה שלכם שבודקת אם המפלצת חיה
                {
                    tile->clearEntity();
                }
            }
        }
    }
}

// החזרת המשבצת הכי שמאלית בשורה מסוימת
Tile* Board::getLeftmostTileInRow(int row) const {
    Tile* leftmost = nullptr;
    int minQ = std::numeric_limits<int>::max();

    for (const auto& [coords, tile] : m_grid) {
        if (coords.second == row) { // אם אנחנו בשורה המבוקשת
            if (coords.first < minQ) {
                minQ = coords.first;
                leftmost = tile.get();
            }
        }
    }
    return leftmost;
}

// החזרת המשבצת הכי ימנית בשורה מסוימת
Tile* Board::getRightmostTileInRow(int row) const {
    Tile* rightmost = nullptr;
    int maxQ = std::numeric_limits<int>::min();

    for (const auto& [coords, tile] : m_grid) {
        if (coords.second == row) { // אם אנחנו בשורה המבוקשת
            if (coords.first > maxQ) {
                maxQ = coords.first;
                rightmost = tile.get();
            }
        }
    }
    return rightmost;
}

Tile* Board::AI_FindBestTargetForMonster(Monster* monster)
{
    if (!monster || !monster->isAlive()) return nullptr;

    // 1. נדליק את ה-Highlights בעזרת הפונקציה הקיימת שלך
    highlightNeighbors(monster);

    Tile* bestAttackTarget = nullptr;
    Tile* bestMoveTarget = nullptr;

    // 2. נעבור על ה-grid (הפרטי של הלוח!) ונבדוק מה הוא האיר
    for (auto& [coords, tile] : m_grid)
    {
        if (!tile->isHighlighted()) continue;

        // עדיפות א': מצאנו אויב בטווח המואר!
        if (tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide())
        {
            bestAttackTarget = tile.get();
            break; // מצאנו מטרה, אין צורך להמשיך לחפש
        }

        // עדיפות ב': משבצת פנויה שמתקדמת שמאלה (Q קטן יותר)
        //if (!tile->hasEntity() && tile->isPassable())
        if (!tile->hasEntity() && tile->isPassableFor(monster))
        {
            if (!bestMoveTarget || tile->getQ() < bestMoveTarget->getQ())
            {
                bestMoveTarget = tile.get();
            }
        }
    }

    // ננקה את הלוח מיד כדי שלא יישאר צבוע
    clearHighlights();

    // נחזיר קודם כל תקיפה, ואם אין - תנועה
    return bestAttackTarget ? bestAttackTarget : bestMoveTarget;
}
//
//void Board::AI_ExecuteAction(Monster* monster, Tile* targetTile)
//{
//    if (!monster || !targetTile) return;
//
//    // 1. אם יש אויב במשבצת היעד -> תקיפה
//    if (targetTile->hasEntity() && targetTile->getEntity()->getSide() != monster->getSide())
//    {
//        monster->attack(targetTile->getEntity());
//        if (!targetTile->getEntity()->isAlive()) {
//            targetTile->clearEntity();
//        }
//    }
//    // 2. אם המשבצת פנויה -> תנועה
//    else if (!targetTile->hasEntity() && targetTile->isPassable())
//    {
//        // מוצאים את המשבצת הנוכחית של המפלצת כדי לנקות אותה
//        auto sourceIt = m_grid.find({ monster->getQ(), monster->getRow() });
//        if (sourceIt != m_grid.end())
//        {
//            Tile* sourceTile = sourceIt->second.get();
//            targetTile->setMonster(monster); // השמה ביעד
//            sourceTile->clearEntity();       // ניקוי המקור
//
//            // עדכון המיקום הפיזי והגרפי של המפלצת
//            monster->moveTo(targetTile->getQ(), targetTile->getRow(),
//                tileToScreen(targetTile->getQ(), targetTile->getRow()));
//        }
//    }
//
//    // חובה: מורידים למפלצת נקודת פעולה כדי שהלולאה לא תהיה אינסופית!
//    //monster->useAction(); // או איך שנקראת אצלך המתודה שמורידה Action (למשל m_actionsLeft--)
//}

bool Board::AI_SpawnMonster(Monster* monster, PlayerSide side)
{
    if (!monster || monster->isOnBoard()) return false;

    int minQ = (side == PlayerSide::Left) ? 0 : 12;
    int maxQ = (side == PlayerSide::Left) ? 1 : 13;

    std::vector<Tile*> availableSpawnTiles;

    for (auto& [coords, tile] : m_grid)
    {
        if (coords.first >= minQ && coords.first <= maxQ)
        {
            //if (!tile->hasEntity() && tile->isPassable())
            if (!tile->hasEntity() && tile->isPassableFor(monster))
            {
                availableSpawnTiles.push_back(tile.get());
            }
        }
    }

    if (availableSpawnTiles.empty()) return false;

    Tile* targetTile = availableSpawnTiles[rand() % availableSpawnTiles.size()];

    // שורה אחת שמחליפה את כל כפל הקוד הפיזי של הזימון!
    return spawnMonsterOnTile(monster, targetTile);
}

bool Board::spawnMonsterOnTile(Monster* monster, Tile* targetTile)
{
    //if (!monster || !targetTile || targetTile->hasEntity() || !targetTile->isPassable()) return false;
    if (!monster || !targetTile || targetTile->hasEntity() || !targetTile->isPassableFor(monster)) return false;
    targetTile->setEntity(monster);
    monster->spawnOnBoard(targetTile->getQ(), targetTile->getRow(),
        tileToScreen(targetTile->getQ(), targetTile->getRow()));
    return true;
}

void Board::performAction(Monster* monster, Tile* targetTile)
{
    if (!monster || !targetTile) return;

    // 1. תקיפה
    if (targetTile->hasEntity() && targetTile->getEntity()->getSide() != monster->getSide())
    {
        monster->attack(targetTile->getEntity());
        if (!targetTile->getEntity()->isAlive()) {
            targetTile->clearEntity();
        }
    }
    // 2. תנועה
    //else if (!targetTile->hasEntity() && targetTile->isPassable())
    else if (!targetTile->hasEntity() && targetTile->isPassableFor(monster))
    {
        auto sourceIt = m_grid.find({ monster->getQ(), monster->getRow() });
        if (sourceIt != m_grid.end())
        {
            Tile* sourceTile = sourceIt->second.get();
            //targetTile->setMonster(monster);
            targetTile->setEntity(monster);
            sourceTile->clearEntity();

            monster->moveTo(targetTile->getQ(), targetTile->getRow(),
                tileToScreen(targetTile->getQ(), targetTile->getRow()));
        }
    }
}

void Board::update(float dt)
{
    for (auto& [coords, tile] : m_grid)
    {
        if (auto entity = tile->getEntity())
        {
            entity->update(dt);
            
        }
    }
}

bool Board::isAnimating() const
{
    for (auto const& [coords, tile] : m_grid)
    {
        if (auto entity = tile->getEntity())
        {
            if (entity->getType() == EntityType::Monster)
            {
                // עכשיו אנחנו בטוחים שזו מפלצת, אז אפשר להמיר בבטחה
                Monster* monster = static_cast<Monster*>(entity);
                if (monster->isMoving()) return true;
            }

        }
    }
    return false; // <--- חסר לך את זה! אם אף אחד לא זז, מחזירים שקר
}