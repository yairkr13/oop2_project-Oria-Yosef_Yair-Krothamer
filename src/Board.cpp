#include "Board.h"
#include "LavaTile.h"
#include "Hole.h"
#include "PanicPoint.h"
#include <random>
#include "Constants.h"
//HEY2
//hiiiiiiiii
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
    float width = std::sqrt(3.f) * Config::TILE_RADIUS;
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
            float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;

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
    //        float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;
    //        m_grid[{q, row}] = std::make_unique<LavaTile>(q, row, sf::Vector2f(x, y));
    //    }
    //    //ליצור פונקציה שפשוט מביאה מישהו מהאקראים !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //    // 2 המיקומים הבאים (2 ו-3) יהיו חורים
    //    for (int i = 2; i < 4; ++i)
    //    {
    //        auto [q, row] = allCoords[i];
    //        float x = START_X + (width / 2.f) * q;
    //        float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;
    //        m_grid[{q, row}] = std::make_unique<Hole>(q, row, sf::Vector2f(x, y));
    //    }
    //}
}
//void Board::createBoard()
//{
//    float width = std::sqrt(3.f) * Config::TILE_RADIUS;
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
//            float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;
//
//            sf::Vector2f physicalPosition(x, y);
//            m_grid[{q, row}] = std::make_unique<Tile>(q, row, physicalPosition);
//        }
//    }
//}

void Board::draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const
{
    // 1. ציור כל המשבצות עצמן
    for (auto const& [coords, tile] : m_grid)
    {
        tile->draw(window, currentTurnSide);
        /* if (tile->hasHeart()) {
             tile->getHeart()->draw(window);
         }*/
    }

    // 2. ציור כל הישויות שנמצאות על המשבצות
    //for (auto const& [coords, tile] : m_grid)
    //{
    //    // lock() ממיר את weak_ptr ל-shared_ptr זמני. אם לא קיים - מחזיר null
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
    float width = std::sqrt(3.f) * Config::TILE_RADIUS;

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
        //std::random_device rd;
        //std::mt19937 g(rd());
        std::shuffle(allCoords.begin(), allCoords.end(), rng());

        // 2 משבצות לבה
        for (int i = 0; i < 2; ++i) {
            auto [q, row] = allCoords[i];
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;
            m_grid[{q, row}] = std::make_unique<LavaTile>(q, row, sf::Vector2f(x, y));
        }

        // 2 משבצות חור
        for (int i = 2; i < 4; ++i) {
            auto [q, row] = allCoords[i];
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;
            m_grid[{q, row}] = std::make_unique<Hole>(q, row, sf::Vector2f(x, y));
        }

        // 1 משבצת פאניקה שמקבלת את הלבבות שהוזרקו לפונקציה!
        for (int i = 4; i < 5; ++i) {
            auto [q, row] = allCoords[i];
            float x = START_X + (width / 2.f) * q;
            float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;
            m_grid[{q, row}] = std::make_unique<PanicPoint>(q, row, sf::Vector2f(x, y), p1Heart, p2Heart);
        }
    }
}

//choosePlace

void Board::handleClick(const sf::Vector2f& pos, PlayerSide currentSide)
{
    // 1. מציאת המשבצת עליה לחצו
    //make this not for loop- eith the find function!!!!!!!!!!!!!!!!!!
    /*Tile* clickedTile = nullptr;
    for (auto& [coords, tile] : m_grid)
    {
        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
        float dx = pos.x - tileCenter.x;
        float dy = pos.y - tileCenter.y;

        if ((dx * dx + dy * dy) <= (Config::TILE_RADIUS * Config::TILE_RADIUS))
        {
            clickedTile = tile.get();
            break;
        }
    }*/
    // 1. מציאת המשבצת עליה לחצו - במקום לסרוק את כל ה-grid ולחשב מרחק לכל tile,
    // ממירים ישירות פיקסלים -> (q,row) ומחפשים ב-find() (O(log n) במקום O(n)).
    auto [q, row] = screenToTile(pos);
    auto tileIt = m_grid.find({ q, row });
    Tile* clickedTile = (tileIt != m_grid.end()) ? tileIt->second.get() : nullptr;


    if (!clickedTile) return;

    // 2. מציאת המפלצת שנבחרה קודם לכן
    Monster* selectedMonster = nullptr;
    for (auto& [coords, tile] : m_grid)
    {
        if (auto entity = tile->getEntity()) // שלב א: לוקחים את הישות הכללית
        {
            if (entity->isSelected())//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            {
                selectedMonster = static_cast<Monster*>(entity); // אבל שימו לב - selectedMonster חייב עדיין להיות Monster*
                break;
            }
            // שלב ב: הלוח שואל "האם אתה מפלצת?" 
            //if (entity->getType() == EntityType::Monster)
            //{
            //    // עכשיו אנחנו בטוחים שזו מפלצת, אז אפשר להמיר בבטחה
            //    Monster* monster = static_cast<Monster*>(entity);

            //    if (monster->isSelected())
            //    {
            //        selectedMonster = monster;
            //        break;
            //    }
            //}
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
            // בודקים אם הישות ניתנת לבחירה (מפלצת - כן, לב - לא)
            if (entity->isSelectable())
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
//        if ((dx * dx + dy * dy) <= (Config::TILE_RADIUS * Config::TILE_RADIUS))
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
//    // 1. קודם כל, מוצאים את משבצת הריצפה (Tile) עליה לחצו
//    Tile* clickedTile = nullptr;
//    for (auto& [coords, tile] : m_grid)
//    {
//        sf::Vector2f tileCenter = tileToScreen(coords.first, coords.second);
//        float dx = pos.x - tileCenter.x;
//        float dy = pos.y - tileCenter.y;
//
//        // בדיקת מרחק פשוטה כדי לראות אם הלחיצה בטווח המשבצת
//        if ((dx * dx + dy * dy) <= (Config::TILE_RADIUS * Config::TILE_RADIUS))
//        {
//            clickedTile = tile.get();
//            break;
//        }
//    }
//
//    // אם לא נמצאה משבצת, אין מה לעשות
//    if (!clickedTile) return;
//
//    // 2. מוצאים אם יש מפלצת שכבר נבחרה מקודם
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
//            //monster->setScreenPosition(tilePos); // <--- זה מעדכן רק ויזואלית!
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
//    // 3. מבצעים את הפעולה:
//    if (selectedMonster)
//    {
//        if (clickedTile->isHighlighted() && !clickedTile->hasEntity())
//        {
//            clickedTile->setMonster(selectedMonster);     // שמים את המפלצת במקום החדש
//            //selectedMonsterTile->setMonster(nullptr);     // מנקים ממקום הישן
//            selectedMonsterTile->clearEntity(); // <--- התיקון כאן: שימוש בפונקציית הניקוי החדשה
//            // מעדכנים גם את ה-Q וה-Row, כדי שפונקציית draw תדע "איפה" היא נמצאת פנימית!
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
//        // בכל מקרה, בסוף הפעולה (בין אם תנועה), מבטלים את הבחירה
//        selectedMonster->setSelected(false);
//        clearHighlights();
//    }
//    else
//    {
//        // אם לא נבחרה שום מפלצת, בודקים אם לחצו על מפלצת כדי לבחור בה
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
                pos.y - tileToScreen(coords.first, coords.second).y) < Config::TILE_RADIUS)
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
//                pos.y - tileToScreen(coords.first, coords.second).y) < Config::TILE_RADIUS)
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
//        if ((dx * dx + dy * dy) <= (Config::TILE_RADIUS * Config::TILE_RADIUS))
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
// שכבה 1: לוגיקה טהורה - "אילו tiles המפלצת יכולה להגיע/לתקוף אליהם", בלי לצייר כלום.
// זהה בדיוק ל-BFS שהיה בתוך highlightNeighbors, רק שבמקום setHighlighted() כל tile
// חוקי פשוט נכנס לרשימה שמוחזרת.
//
// הערה: נשמרה כאן במכוון "תכונה" קיימת מהגרסה המקורית - tile של חור עם אויב מעופף
// עליו אף פעם לא מסומן ב-visited (רק tiles "רגילים" מסומנים). זה אומר שתיאורטית, אם
// אותו חור נגיש משני כיוונים שונים באותו BFS, הוא עלול להיכנס לרשימה פעמיים. זו לא
// באגה חדשה שהוספתי - היא הייתה קיימת גם בגרסה הישנה (רק שם זה היה "לצבוע פעמיים"
// שלא נראה למשתמש, ואילו כאן זה "רשימה עם כפילות אפשרית"). אם תרצו, אפשר לתקן בקלות
// ע"י סימון visited גם על tiles של חור - תגיד לי אם לתקן את זה גם.
// ה-BFS המשותף (מעבר יחיד) שגם getReachableTiles וגם getPathTo נשענים עליו.
// זהה לגמרי ל-BFS הקודם שהיה בתוך getReachableTiles - השינוי היחיד: לצד
// visited (מרחק, פנימי בלבד - לא מוחזר החוצה), נוסף outParent שזוכר לכל
// tile "מאיפה הגעתי אליו" - זה מה שמאפשר ל-getPathTo לשחזר מסלול מדורג
// בלי להריץ שוב את בדיקות החסימה (חור/תעופה/אויב).
//
// אותה הערה כמו קודם עדיין רלוונטית: tile של חור עם אויב מעופף עליו לא
// מסומן ב-visited (רק tiles "רגילים" מסומנים), אז תיאורטית יכול להיכנס
// לרשימה/למפת ה-parent פעמיים אם הוא נגיש משני כיוונים. זו לא באגה חדשה.
void Board::computeReachability(Monster* monster,
    std::vector<Tile*>& outReachable,
    std::map<std::pair<int, int>, std::pair<int, int>>& outParent) const
{
    outReachable.clear();
    outParent.clear();
    if (!monster) return;

    int q = monster->getQ();
    int row = monster->getRow();
    int range = monster->getRange();

    std::pair<int, int> offsets[] = {
        {-2,  0}, {+2,  0},
        {-1, -1}, {+1, -1},
        {-1, +1}, {+1, +1}
    };

    std::map<std::pair<int, int>, int> visited; // מרחק בלבד - פנימי לחישוב, לא מוחזר
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

                Tile* tile = it->second.get();

                // --- בדיקת חור ותעופה ---
                if (tile->isHole() && !monster->canFly())
                {
                    if (tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide())
                    {
                        outReachable.push_back(tile); // אויב מעופף מעל הבור - ניתן לתקוף
                        outParent[neighbor] = { cq, cr };
                    }
                    continue; // מפלצת קרקע לא יכולה לעבור/לעמוד על החור
                }
                visited[neighbor] = dist + 1;
                outParent[neighbor] = { cq, cr };

                // --- זיהוי אויבים/לבבות שחוסמים המשך תנועה ---
                bool blockedByEnemy = tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide();
                outReachable.push_back(tile);

                if (blockedByEnemy)
                {
                    if (!monster->canFly()) continue; // האויב חוסם את המשך הדרך
                }

                nextFrontier.push_back(neighbor);
            }
        }
        frontier = std::move(nextFrontier);
    }
}

std::vector<Tile*> Board::getReachableTiles(Monster* monster) const
{
    std::vector<Tile*> reachable;
    std::map<std::pair<int, int>, std::pair<int, int>> parent; // לא בשימוש כאן, רק כי computeReachability דורש אותו
    computeReachability(monster, reachable, parent);
    return reachable;
}

// שחזור המסלול: הולכים אחורה מה-target דרך outParent עד שמגיעים למקור, ואז
// הופכים את הסדר (כי בנינו אותו מהסוף להתחלה). אם target לא הופיע ב-parent
// בכלל - זה אומר שהוא לא נגיש (או שהוא עצמו נקודת המוצא), ומוחזרת רשימה ריקה.
std::vector<Tile*> Board::getPathTo(Monster* monster, Tile* target) const
{
    std::vector<Tile*> path;
    if (!monster || !target) return path;

    std::vector<Tile*> reachable;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    computeReachability(monster, reachable, parent);

    std::pair<int, int> sourceCoords = { monster->getQ(), monster->getRow() };
    std::pair<int, int> targetCoords = { target->getQ(), target->getRow() };

    if (targetCoords == sourceCoords) return path; // כבר שם
    if (!parent.count(targetCoords)) return path;   // לא נגיש - לא אמור לקרות בפועל

    std::vector<std::pair<int, int>> reversedCoords;
    std::pair<int, int> cur = targetCoords;
    while (cur != sourceCoords)
    {
        reversedCoords.push_back(cur);
        cur = parent.at(cur);
    }
    std::reverse(reversedCoords.begin(), reversedCoords.end());

    for (auto& coords : reversedCoords)
    {
        auto it = m_grid.find(coords);
        if (it != m_grid.end())
            path.push_back(it->second.get());
    }

    return path;
}

// שכבה 2: wrapper דק - קורא לשאילתה הטהורה למעלה, ורק אחראי על הצביעה (side effect
// ויזואלי). זו הפונקציה שהקליק האנושי קורא לה. AIPlayer, לעומת זאת, יוכל בעתיד לקרוא
// ישירות ל-getReachableTiles ולא לצייר כלום - הוא לא צריך צביעה, רק את הרשימה.
void Board::highlightNeighbors(Monster* monster)
{
    if (!monster) return;

    for (Tile* tile : getReachableTiles(monster))
    {
        bool isEnemy = tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide();
        if (isEnemy)
            tile->setHighlighted(true, sf::Color(255, 90, 90, 180)); // אדום - ניתן לתקוף
        else
            tile->setHighlighted(true); // ירוק (ברירת המחדל) - ניתן לזוז
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
    float width = std::sqrt(3.f) * Config::TILE_RADIUS;

    // חייב להיות תואם ל-createBoard!
    float x = START_X + (width / 2.f) * q;
    float y = START_Y + (1.5f * Config::TILE_RADIUS) * row;

    return { x + Config::TILE_RADIUS, y + Config::TILE_RADIUS };
}

void Board::updateTileEffects()
{
    for (auto& [coords, tile] : m_grid)
    {
        tile->applyTileEffect(); // פולימורפיזם בפעולה! משבצת רגילה לא תעשה כלום, לבה תוריד חיים.

        // אם המפלצת מתה מהאפקט (למשל מהלבה), ננקה אותה מהמשבצת
        if (auto entity = tile->getEntity())
        {
            if (!entity->isAlive()) // משתמש במתודה שלכם שבודקת אם המפלצת חיה
            {
                tile->clearEntity();
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

// AI_FindBestTargetForMonster עברה ל-AIPlayer::findBestTarget - היא הייתה מכילה
// היוריסטיקה (תקיפה > תנועה שמאלה), לא שאילתה עובדתית, אז לא הייתה שייכת ל-Board.
// AIPlayer עכשיו קורא ל-getReachableTiles() (למעלה בקובץ) בעצמו במקומה.
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

    // תוקן: הבדיקה חייבת לקרות *לפני* שמשתמשים ב-size()-1 (אחרת underflow על size_t
    // כשהוקטור ריק), וגם הייתה כאן הצהרה כפולה של targetTile שלא מתקמפלת.
    if (availableSpawnTiles.empty()) return false;

    std::uniform_int_distribution<size_t> dist(0, availableSpawnTiles.size() - 1);
    Tile* targetTile = availableSpawnTiles[dist(rng())];

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

            // בונים את המסלול המדורג (משבצת-משבצת) במקום לקפוץ בקו ישר ליעד
            std::vector<Tile*> path = getPathTo(monster, targetTile);
            std::vector<sf::Vector2f> pathScreenPositions;
            pathScreenPositions.reserve(path.size());
            for (Tile* step : path)
                pathScreenPositions.push_back(tileToScreen(step->getQ(), step->getRow()));

            // רשת ביטחון: אם משום מה לא נמצא מסלול (לא אמור לקרות, כי targetTile
            // כבר אושר כנגיש), נופלים חזרה על תזוזה ישירה כדי שהמפלצת לא "תיתקע"
            if (pathScreenPositions.empty())
                pathScreenPositions.push_back(tileToScreen(targetTile->getQ(), targetTile->getRow()));

            monster->moveAlongPath(targetTile->getQ(), targetTile->getRow(), pathScreenPositions);
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
            if (entity->isMoving()) return true;
            //if (entity->getType() == EntityType::Monster)
            //{
            //    // עכשיו אנחנו בטוחים שזו מפלצת, אז אפשר להמיר בבטחה
            //    Monster* monster = static_cast<Monster*>(entity);
            //    if (monster->isMoving()) return true;
            //}

        }
    }
    return false; // <--- חסר לך את זה! אם אף אחד לא זז, מחזירים שקר
}

std::pair<int, int> Board::screenToTile(const sf::Vector2f& pos) const
{
    // מזיזים את pos למערכת הצירים שעליה tileToScreen בנוי - מחסירים את נקודת
    // המוצא (START_X/START_Y) וגם את TILE_RADIUS ש-tileToScreen מוסיף בסוף כדי
    // למרכז את המשושה (ה-shape עצמו ממוקם לפי הפינה השמאלית-עליונה, לא המרכז).
    float x = pos.x - START_X - Config::TILE_RADIUS;
    float y = pos.y - START_Y - Config::TILE_RADIUS;

    // 1. המרה ל-axial שברי (pointy-top hex, size = TILE_RADIUS)
    float q_axial_frac = (std::sqrt(3.f) / 3.f * x - 1.f / 3.f * y) / Config::TILE_RADIUS;
    float r_axial_frac = (2.f / 3.f * y) / Config::TILE_RADIUS;

    // 2. עיגול מדויק דרך cube coordinates
    float x_cube = q_axial_frac;
    float z_cube = r_axial_frac;
    float y_cube = -x_cube - z_cube;

    int rx = static_cast<int>(std::round(x_cube));
    int ry = static_cast<int>(std::round(y_cube));
    int rz = static_cast<int>(std::round(z_cube));

    float x_diff = std::abs(rx - x_cube);
    float y_diff = std::abs(ry - y_cube);
    float z_diff = std::abs(rz - z_cube);

    if (x_diff > y_diff && x_diff > z_diff)
        rx = -ry - rz;
    else if (y_diff > z_diff)
        ry = -rx - rz;
    else
        rz = -rx - ry;

    int qAxial = rx;
    int row = rz; // rz == r_axial המעוגל, שהוא בדיוק ה-row שלנו

    // 3. axial -> doubled coordinates (מערכת ה-q שהפרויקט משתמש בה בפועל)
    int qDouble = 2 * qAxial + row;

    return { qDouble, row };
}
