#include "Monsters/Barzilla.h"
#include "Attacks/SplashAttackAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float FIRE_BLAST_DURATION = 0.5f; // seconds for the blast to travel from Barzilla to its target

    // Same reasoning as Mozzy's ACID_SPLASH_THICKNESS: kept proportional to
    // Config::MONSTER_BOARD_SIZE (the project's existing on-board sizing
    // reference) rather than FireBlast.png's raw pixel height.
    constexpr float FIRE_BLAST_THICKNESS = Config::MONSTER_BOARD_SIZE * 0.5f;
}

Barzilla::Barzilla(PlayerSide side)
    : Monster(side, "Barzilla", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Red, "barzilla")
{
    //TextureManager::getInstance().loadTexture("barzilla_r", "resources/Monster/Barzilla/Barzilla_R.png");
    //TextureManager::getInstance().loadTexture("barzilla_l", "resources/Monster/Barzilla/Barzilla_L.png");
    //TextureManager::getInstance().loadTexture("barzilla_card_r", "resources/Monster/Barzilla/Barzilla_card_R.png");
    //TextureManager::getInstance().loadTexture("barzilla_card_l", "resources/Monster/Barzilla/Barzilla_card_L.png");
}

// Empowered Attack: doubles the damage of the next successful attack only.
// The multiplier is read here, at the moment an attack actually resolves -
// since damage now applies at animation-impact time (see
// createAttackAnimation/Tile::receiveAttackFrom), this is naturally also
// exactly when the bonus is consumed, with no extra plumbing needed for
// that timing to line up. This is also the Special's actual commit point:
// arming it (onSpecialAbility, below) deliberately left the action/cooldown
// untouched, so committing them here - only once the empowered attack has
// genuinely happened - is what makes "select Special" and "use Special"
// two different events instead of one.
void Barzilla::attack(BoardEntity* target)
{
    int damage = m_empoweredAttack ? m_attackDamage * 2 : m_attackDamage;

    if (m_empoweredAttack)
    {
        m_empoweredAttack = false;
        m_specialCooldown = m_baseCooldown;
    }

    target->takeDamage(damage);
    useAction(); // an attack always costs an action, empowered or not
}

// Arms the bonus for the next attack - does not itself consume an action or
// touch the cooldown (see Monster::useSpecialAbility /
// specialAbilityCommitsOnSelect()). Selecting the Special is not using it.
void Barzilla::onSpecialAbility(Board& board, BoardEntity* target)
{
    m_empoweredAttack = true;
}

void Barzilla::onTurnBoundary()
{
    BoardEntity::onTurnBoundary();
    // Unconsumed bonus does not carry into a future turn.
    m_empoweredAttack = false;
}

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    // Same "grows/reveals from attacker toward target" mechanism as Mozzy's
    // acid splash - only the texture (and its proportions) differ, so this
    // reuses SplashAttackAnimation as-is rather than duplicating it.
    const sf::Texture& fireBlastTexture = AssetsManager::getInstance().getTexture("fire_blast");
    return std::make_unique<SplashAttackAnimation>(
        fireBlastTexture, m_screenPos, target->getScreenPosition(), FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}
