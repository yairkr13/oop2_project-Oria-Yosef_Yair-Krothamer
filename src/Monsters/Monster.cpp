#include "Monsters/Monster.h"
#include "Constants.h"
#include "AssetsManager.h"
#include "Attacks/AttackAnimation.h" // complete type needed for the destructor and m_attackAnimation below

// Defined here (not "= default" inline in the header): destroying
// m_attackAnimation (a unique_ptr<AttackAnimation>) requires the complete
// AttackAnimation type, which is only forward-declared in Monster.h.
Monster::~Monster() = default;

Monster::Monster(PlayerSide side, const std::string& name, int health, int attackPower, int range, int baseCooldown/*, int cost*/, int q, int row, sf::Color color, const std::string& textureKey, bool flying)
    : BoardEntity(q, row, {}, health),
    m_side(side), m_name(name), m_attackDamage(attackPower),
    m_range(range)/*, m_cost(cost)*/, m_color(color), m_textureKey(textureKey), m_flying(flying),
    m_baseCooldown(baseCooldown), m_specialCooldown(baseCooldown),
    m_sprite(AssetsManager::getInstance().getTexture(m_textureKey))
{
    try
    {
        const sf::Texture& texture = m_sprite.getTexture();
        //m_sprite(texture);

        // origin at texture center
        m_sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f });

        // scale relative to on-board size
        float maxTextureDim = std::max(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));
        m_baseScale = Config::MONSTER_BOARD_SIZE / maxTextureDim;

        //m_sprite.setScale({ m_baseScale, m_baseScale });
        m_hasTexture = true;
    }
    catch (...)
    {
        // no texture available, draw fallback shape in draw()
        m_hasTexture = false;
    }
}

std::unique_ptr<SpriteSheet> Monster::configureSpriteSheet(const std::string& textureKey, int columns, int rows, float frameDuration, bool looping) const
{
    const sf::Texture& texture = AssetsManager::getInstance().getTexture(textureKey);
    return std::make_unique<SpriteSheet>(texture, columns, rows, frameDuration, Config::MONSTER_BOARD_SIZE, looping);
}

namespace
{
    // Priority for each of Monster's four animation states, handed to
    // SpriteAnimator::addState (lower wins when more than one state's own
    // isActive() is simultaneously true) - this is the data-form of what
    // used to be a hardcoded if/else chain in draw() (die beats attack
    // beats walk beats idle). Registration order no longer matters at all
    // (see addAnimationState/SpriteAnimator) - each concrete monster's
    // constructor is free to call the four setters below in any order, as
    // they already do inconsistently.
    constexpr int DIE_PRIORITY = 0;
    constexpr int ATTACK_PRIORITY = 1;
    constexpr int WALK_PRIORITY = 2;
    constexpr int IDLE_PRIORITY = 3;
}

void Monster::addAnimationState(AnimState id, std::unique_ptr<SpriteSheet> sheet,
    std::function<bool()> isActive, int priority)
{
    m_animator.addState(static_cast<int>(id), std::move(sheet), std::move(isActive), priority);
}

void Monster::setWalkAnimation(const std::string& walkTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Walk, configureSpriteSheet(walkTextureKey, columns, rows, frameDuration),
        [this]() { return m_isMoving; }, WALK_PRIORITY);
}

void Monster::setAttackSpriteAnimation(const std::string& attackTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Attack, configureSpriteSheet(attackTextureKey, columns, rows, frameDuration),
        [this]() { return isAttacking(); }, ATTACK_PRIORITY);
}

void Monster::setIdleSpriteAnimation(const std::string& idleTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Idle, configureSpriteSheet(idleTextureKey, columns, rows, frameDuration),
        [this]() { return !m_isMoving && !isAttacking(); }, IDLE_PRIORITY);
}

void Monster::setDieSpriteAnimation(const std::string& dieTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Die, configureSpriteSheet(dieTextureKey, columns, rows, frameDuration, /*looping=*/false),
        [this]() { return !isAlive(); }, DIE_PRIORITY);
}

bool Monster::isDying() const
{
    return !isAlive() && !m_animator.isStateFinished(static_cast<int>(AnimState::Die));
}

bool Monster::isReadyForRemoval() const
{
    if (isAlive()) return false;
    return m_animator.isStateFinished(static_cast<int>(AnimState::Die));
}

void Monster::draw(sf::RenderWindow& window) const
{
    if (m_q == -1 && m_row == -1) return;

    if (m_hasTexture)
    {
        // Which of Monster's four registered states (if any) is currently
        // showing was already decided once this frame, in update() below
        // (see m_animator.update()) - draw() never re-derives that
        // decision itself, it only ever reads it back via hasActiveState/
        // applyCurrentFrame/getActiveBaseScale. That's the one behavioral
        // difference from the old hardcoded if/else chain this replaced:
        // previously "who's active" was computed independently here AND in
        // update(), with nothing keeping the two in sync beyond careful
        // hand-editing; now there is exactly one place that decides it.
        if (m_animator.hasActiveState())
        {
            // Delegates texture/rect/origin entirely to SpriteSheet (via
            // SpriteAnimator) - the sprite's own screen position/scale
            // below never changes because of this, only which pixels of
            // which texture it shows, so switching frames can't make it
            // jump around.
            m_animator.applyCurrentFrame(m_sprite);
        }
        else if (m_animator.hasAnyState())
        {
            // At least one state is configured but none applies right now
            // (e.g. a monster with only a walk sheet, currently standing
            // still with no idle sheet of its own) - fall back to the
            // static sprite/origin explicitly, since the sprite's texture/
            // rect/origin were left however the last active sheet set them.
            const sf::Texture& idleTexture = AssetsManager::getInstance().getTexture(m_textureKey);
            m_sprite.setTexture(idleTexture, true); // reset rect back to the full static image
            m_sprite.setOrigin({ idleTexture.getSize().x / 2.f, idleTexture.getSize().y / 2.f });
        }

        // A sheet in use draws with its own base scale (derived from one of
        // its frames' real pixel size - see SpriteSheet::getBaseScale())
        // rather than m_baseScale (derived from the static image's size),
        // so a monster reads as the same on-board size whether idle or
        // sheet-animated, even though the textures aren't the same native
        // resolution.
        float scale = m_animator.hasActiveState() ? m_animator.getActiveBaseScale() : m_baseScale;
        float currentScaleX = (m_side == PlayerSide::Right) ? -scale : scale;
        m_sprite.setPosition(m_screenPos);
        m_sprite.setScale({ currentScaleX, scale });

        window.draw(m_sprite);
    }
    else
    {
        sf::CircleShape circle(Config::MONSTER_BOARD_SIZE / 2.f);
        circle.setFillColor(m_color);
        circle.setOrigin({ 16.f, 16.f });
        circle.setPosition(m_screenPos);
        window.draw(circle);
    }
    //if (m_side != currentTurnSide)
    //{
    //    drawHealthBar(window); // already a no-op while dead - see BoardEntity::drawHealthBar
    //}
    if (isAlive())
    {
        // Skipped while dying (see isDying()) - an actions-left count has
        // no meaning floating over a monster that's already dead and
        // playing its death animation.
		drawHealthBar(window);
        drawActionsLeft(window);
    }

    // This monster draws its own in-flight attack animation, if any -
    // ownership mirrors movement (see m_pathQueue/m_isMoving): Board never
    // draws this directly, it only draws entities, and this is part of how
    // this entity draws itself.
    if (m_attackAnimation)
        m_attackAnimation->draw(window);

    // Same reasoning, separate slot: an incoming Special Ability effect
    // (e.g. Muffintop's Heal effect playing on this monster).
    if (m_specialAnimation)
        m_specialAnimation->draw(window);
}
void Monster::drawActionsLeft(sf::RenderWindow& window) const
{
    sf::Text actionText(AssetsManager::getInstance().getFont("Lilita"));
    actionText.setString(std::to_string(m_actionsLeft));
    actionText.setCharacterSize(14);
    actionText.setFillColor(sf::Color::Yellow);

    actionText.setPosition({ m_screenPos.x - 15.f, m_screenPos.y + Config::TILE_RADIUS - 20.f });

    window.draw(actionText);
}

void Monster::resetActions()
{
    // Whether or not this monster was frozen, its blocked turn (if any) has
    // now concluded - clear the flag and restore normal actions. See
    // applyFreeze()/isFrozen() for why this alone is sufficient: freeze
    // itself already zeroed m_actionsLeft immediately at cast time, and
    // resetActions() only ever runs again once per owner-turn-end, which is
    // exactly when that one blocked turn is over.
    m_frozen = false;
    m_actionsLeft = 2;
	if (m_specialCooldown > 0)
		m_specialCooldown--;
}

void Monster::applyFreeze()
{
    m_frozen = true;
    m_actionsLeft = 0;
}

void Monster::walkTo(const sf::Vector2f& targetScreenPos)
{
    // צעד יחיד = תור עם פריט אחד. אין יותר m_targetPos נפרד - front() של התור
    // *הוא* היעד הנוכחי, מקור אמת יחיד.
    m_pathQueue.clear();
    m_pathQueue.push_back(targetScreenPos);
    m_isMoving = true;
}

void Monster::moveTo(int q, int row, const sf::Vector2f& screenPos)
{
    if (m_actionsLeft <= 0) return;

    // עדכון המיקום הלוגי מיידי, רק הציור זז בהדרגה דרך update()
    m_q = q;
    m_row = row;
    walkTo(screenPos);  // animate visually instead of teleporting

    useAction();
}

void Monster::moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions)
{
    if (m_actionsLeft <= 0 || pathScreenPositions.empty()) return;

    // בדיוק כמו ב-moveTo: המיקום הלוגי מתעדכן מיידית ליעד הסופי. השינוי היחיד הוא
    // שהציור לא "יטפס" בקו ישר אליו, אלא יעבור דרך כל צעד ברשימה, אחד אחרי השני.
    m_q = finalQ;
    m_row = finalRow;

    m_pathQueue.assign(pathScreenPositions.begin(), pathScreenPositions.end());
    m_isMoving = true;

    useAction();
}

void Monster::update(float dt)
{
    // Advance this monster's own attack animation (if any) independently of
    // movement - it must keep progressing even while this monster itself
    // isn't moving (the usual case: attacker stands still while its splash
    // travels to the target), so this runs before the movement early-return
    // below, not after it.
    if (m_attackAnimation)
    {
        m_attackAnimation->update(dt);
        if (m_attackAnimation->isFinished())
            m_attackAnimation.reset();
    }

    // Same independence as the attack animation above: an incoming Special
    // effect must keep progressing regardless of this monster's own
    // movement/attack state (e.g. Muffintop's Heal effect plays on an ally
    // that is otherwise doing nothing at all).
    if (m_specialAnimation)
    {
        m_specialAnimation->update(dt);
        if (m_specialAnimation->isFinished())
            m_specialAnimation.reset();
    }

    // State-driven sprite-sheet animations (see setWalkAnimation/
    // setAttackSpriteAnimation/setIdleSpriteAnimation/setDieSpriteAnimation):
    // re-evaluates each registered state's own isActive() predicate, once,
    // and decides which single one (if any) is showing this frame -
    // advancing that one's clock and resetting every other back to frame 0
    // so it starts fresh whenever it next becomes active. draw() (above,
    // textually - runs after this each frame) only ever reads that decision
    // back, never recomputes it. No-op for any monster that never called
    // any of the four setters (m_animator.hasAnyState() stays false). Die's
    // predicate (!isAlive()) is permanent once true - it keeps winning
    // forever afterward regardless of what m_isMoving/isAttacking() do.
    m_animator.update(dt);

    // אם לא זזים כרגע, אין מה לעדכן
    if (!m_isMoving || m_pathQueue.empty())
    {
        m_isMoving = false;
        return;
    }

    // 1. וקטור הכיוון והמרחק ליעד הנוכחי - front() של התור, לא משתנה נפרד
    const sf::Vector2f& currentTarget = m_pathQueue.front();
    float dx = currentTarget.x - m_screenPos.x;
    float dy = currentTarget.y - m_screenPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // 2. הגענו לצעד הנוכחי? (מספיק קרוב - פחות מ-5 פיקסלים)
    if (distance < 5.0f)
    {
        m_screenPos = currentTarget; // מיישרים בדיוק לצעד הנוכחי
        m_pathQueue.pop_front();     // סיימנו את הצעד הזה

        if (m_pathQueue.empty())
        {
            // זה היה הצעד האחרון - האנימציה נגמרה
            m_isMoving = false;
        }
        // אחרת: m_isMoving נשאר true, ו-front() הבא ישמש כיעד ב-frame הבא
    }
    else
    {
        // 3. עוד לא הגענו - נזוז צעד קטן לכיוון היעד הנוכחי
        float moveX = (dx / distance) * m_speed * dt;
        float moveY = (dy / distance) * m_speed * dt;

        m_screenPos.x += moveX;
        m_screenPos.y += moveY;
    }

    // 4. עדכון גרפי: מעדכנים את ה-sprite כדי שה-SFML ידע איפה לצייר אותו עכשיו
    m_sprite.setPosition(m_screenPos);
}

bool Monster::isOnBoard() const {
    return m_q != -1 && m_row != -1;
}

bool Monster::useSpecialAbility(Board& board, BoardEntity* target)
{
    if (!canUseSpecialAbilityNow())
        return false;

    onSpecialAbility(board, target); // הפעלת הבונוס הייחודי של המפלצת

    if (specialAbilityCommitsOnSelect())
    {
        useAction();              // צריכת נקודת פעולה
        m_specialCooldown = m_baseCooldown; // כל מפלצת מתאפסת ל-cooldown הבסיסי שלה
    }
    // else: onSpecialAbility() only armed monster-specific state (see
    // Barzilla) - the action/cooldown bookkeeping is deferred until that
    // monster itself decides its Special has actually been used.

    return true;
}

void Monster::attack(BoardEntity* target)
{
    target->takeDamage(m_attackDamage);
    useAction();
}

void Monster::playAttackAnimation(std::unique_ptr<AttackAnimation> animation)
{
    m_attackAnimation = std::move(animation);
}

void Monster::playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation)
{
    m_specialAnimation = std::move(animation);
}