#pragma once
#include "SFML/Graphics.hpp"
#include "Monsters/Monster.h"
#include "BoardEntity.h"
#include "Constants.h"
#include <memory>
#include <optional>

// A single hex cell on the board. Owns its shape/highlight state and at
// most one occupying BoardEntity, and forwards per-frame/per-turn updates
// and tile effects to it.
class Tile
{
public:
	Tile(int q, int row, const sf::Vector2f& position, const sf::Color& color= sf::Color(80, 80, 80, 180)); // default: gray
    virtual ~Tile() = default;
	void draw(sf::RenderWindow& window) const;

	void drawEntity(sf::RenderWindow& window, PlayerSide currentSide) const;

	int getQ() const;
	int getRow() const;

	sf::Vector2f getScreenPosition() const;

	bool isHighlighted() const;

	void setHighlighted(bool highlighted, const sf::Color& highlightColor= sf::Color(150, 220, 150, 180)); // default: light green

    void clearEntity();

    const BoardEntity* getEntity() const;

    BoardEntity* getMutableEntity() const;

    void setEntity(BoardEntity* entity);
    bool hasEntity() const;

    virtual bool isPassableFor(const BoardEntity* entity) const;

    bool isOccupiedByEnemy(PlayerSide mySide) const;
    bool isOccupiedByAlly(PlayerSide mySide) const;

    void receiveAttackFrom(BoardEntity* attacker);

    void tickTurnBoundary();

    void updateEntity(float dt);

    bool isEntityAnimating() const;

    void damageEntity(int amount);

    float scoreAsAttackTarget() const;

    virtual void applyTileEffect();
protected:
    bool m_isPassable; // set directly by Hole's constructor

    virtual std::optional<sf::Color> ownHighlightColor() const;
private:
    sf::CircleShape m_shape;
    sf::Color m_color;
	bool isEntityAlive() const;

	int m_row;
	int m_q;
	bool m_isHighlighted = false;
	BoardEntity* m_entity = nullptr;
};
