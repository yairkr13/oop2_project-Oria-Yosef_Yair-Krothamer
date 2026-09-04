#include "Tiles/Tile.h"


Tile::Tile(int q, int row, const sf::Vector2f& position,const sf::Color& color)// <--- ��� ����� ������ ����� �� ����� ���!
    :m_q(q),
    m_row(row),
    m_isPassable(true),
    m_color(color)
{
    // ������ ������ (m_shape)...

    m_shape.setRadius(Config::TILE_RADIUS);

    // ���� ����� ���: ���� �� 6 ������ ��� �����!
    m_shape.setPointCount(6);

    // ��� �����
    m_shape.setFillColor(m_color); // light gray, semi-transparent

    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color(80, 80, 80, 180));

    // ������ �� ������
    //m_shape.setRotation(sf::degrees(30.f));
    m_shape.setPosition(position);
    //m_shape.setOrigin({ Config::TILE_RADIUS, Config::TILE_RADIUS });
    // ���������: ������ �� ����� ����� ����� ������
    // m_shape.setOrigin(radius, radius);
}

void Tile::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
    if (m_entity != nullptr)
    {
        m_entity->draw(window); // קריאה וירטואלית (אפס RTTI, מהיר לחלוטין)
    }
}

void Tile::setHighlighted(bool highlighted,const sf::Color& highlightColor)
{
    m_isHighlighted = highlighted;
    if (highlighted)
    {
        m_shape.setFillColor(highlightColor); // light green, semi-transparent
        m_shape.setOutlineColor(sf::Color(200, 255, 200, 220));
    }
    else
    {
        m_shape.setFillColor(m_color); // light gray, semi-transparent
        m_shape.setOutlineColor(sf::Color(80, 80, 80, 180));
    }
}

void Tile::setEntity(BoardEntity* entity)
{
    m_entity = entity;
    m_isPassable = false;

    // אם קיבלנו ישות אמיתית, נעדכן אותה שהיא עומדת עלינו
   /* if (m_entity != nullptr)
    {
        m_entity->setCurrentTile(this);
    }*/
    if (m_entity != nullptr)
        m_entity->setCurrentTile(this);
}

void Tile::clearEntity()
{
    if (m_entity != nullptr && m_entity->getCurrentTile() == this)
    {
        m_entity->setCurrentTile(nullptr);
    }
    m_entity = nullptr;
    m_isPassable = true;
}

void Tile::receiveAttackFrom(BoardEntity* attacker)
{
    if (!attacker || m_entity == nullptr) return;

    BoardEntity* defender = m_entity;
    attacker->attack(defender);

    // isReadyForRemoval(), not isAlive() directly - a defender with a
    // configured death animation (see Monster::isReadyForRemoval) is
    // already dead (isAlive() is already false, so it's already
    // unselectable/untargetable/unable to act - see Tile::isOccupiedByEnemy/
    // Monster::canBeSelectedBy) but stays linked to this Tile so that
    // animation can keep playing; Board::update() clears it once it
    // finishes. A defender with no death animation configured is ready
    // immediately, same as before this distinction existed.
    if (defender->isReadyForRemoval())
    {
        //attacker->onKill(defender);
        //defender->onDeath();
        clearEntity();
    }
}