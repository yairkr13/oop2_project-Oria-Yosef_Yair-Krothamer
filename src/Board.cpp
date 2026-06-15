#include "Board.h"

Board::Board() 
{
    createBoard();
}

void Board::createBoard()
{
    float radius = 32.f; // שיניתי ל-32 כדי שיתאים בדיוק ל-RADIUS של ה-Tile שלך
    float width = std::sqrt(3.f) * radius; // הרוחב מקצה לקצה של משושה שפיצי

    float start_x = 50.f;
    float start_y = 50.f;

    // לפי התמונה של Red Blob Games שהעלית:
    int max_rows = 7;  // שורות מ-0 עד 6
    int max_cols = 14; // טורים מ-0 עד 13

    for (int row = 0; row < max_rows; ++row)
    {
        // קסם ה-Double-width: 
        // אם השורה זוגית, הטורים יהיו זוגיים (מתחילים ב-0)
        // אם השורה אי-זוגית, הטורים יהיו אי-זוגיים (מתחילים ב-1)
        int start_col = (row % 2 == 0) ? 0 : 1;

        // אנחנו רצים בקפיצות של 2 בטורים! (q += 2)
        for (int q = start_col; q < max_cols; q += 2)
        {
            // הנוסחאות הרשמיות של Red Blob Games:
            float x = start_x + (width / 2.f) * q;
            float y = start_y + (1.5f * radius) * row;

            sf::Vector2f physicalPosition(x, y);

            // הכנסה ל-Map
            m_grid[{q, row}] = std::make_unique<Tile>(q, row, physicalPosition);
        }
    }
}

void Board::draw(sf::RenderWindow& window) const
{
	for (const auto &pair : m_grid)
	{
        pair.second->draw(window);
	}
}

