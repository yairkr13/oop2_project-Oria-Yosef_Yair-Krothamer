#include "Monsters/Monster.h"
#include "Constants.h"
#include "AssetsManager.h"
#include "Attacks/AttackAnimation.h" // complete type needed for the destructor and m_attackAnimation below
#include "SoundPlayer.h"
#include "SpriteUtils.h"

// Out-of-line: destroying m_attackAnimation needs AttackAnimation's
// complete type, only forward-declared in Monster.h.
Monster::~Monster() = default;

Monster::Monster(PlayerSide side, int health, int attackPower, int range, int baseCooldown/*, int cost*/, int q, int row, sf::Color color, const std::string& textureKey, bool flying)
    : BoardEntity(q, row, {}, health),
    m_side(side), m_attackDamage(attackPower),
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
        m_baseScale = SpriteUtils::maxDimensionScale(texture.getSize(), Config::MONSTER_BOARD_SIZE);

        //m_sprite.setScale({ m_baseScale, m_baseScale });
        m_hasTexture = true;
    }
    catch (...)
    {
        // no texture available, draw fallback shape in draw()
        m_hasTexture = false;
    }
}

// Builds a SpriteSheet sized against Config::MONSTER_BOARD_SIZE - the same
// reference the static sprite's own m_baseScale uses.
std::unique_ptr<SpriteSheet> Monster::configureSpriteSheet(const std::string& textureKey, int columns, int rows, float frameDuration, bool looping) const
{
    const sf::Texture& texture = AssetsManager::getInstance().getTexture(textureKey);
    return std::make_unique<SpriteSheet>(texture, columns, rows, frameDuration, Config::MONSTER_BOARD_SIZE, looping);
}

namespace
{
    // Priority for each animation state (lower wins if more than one is
    // active at once): die beats attack beats walk beats idle.
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

// Looping sheet shown only while isMoving() is true.
void Monster::setWalkAnimation(const std::string& walkTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Walk, configureSpriteSheet(walkTextureKey, columns, rows, frameDuration),
        [this]() { return m_isMoving; }, WALK_PRIORITY);
}

// Looping sheet shown only while isAttacking() is true.
void Monster::setAttackSpriteAnimation(const std::string& attackTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Attack, configureSpriteSheet(attackTextureKey, columns, rows, frameDuration),
        [this]() { return isAttacking(); }, ATTACK_PRIORITY);
}

// Looping sheet shown only when neither moving nor attacking.
void Monster::setIdleSpriteAnimation(const std::string& idleTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Idle, configureSpriteSheet(idleTextureKey, columns, rows, frameDuration),
        [this]() { return !m_isMoving && !isAttacking(); }, IDLE_PRIORITY);
}

// One-shot sheet (non-looping) shown once dead; holds its last frame.
void Monster::setDieSpriteAnimation(const std::string& dieTextureKey, int columns, int rows, float frameDuration)
{
    addAnimationState(AnimState::Die, configureSpriteSheet(dieTextureKey, columns, rows, frameDuration, /*looping=*/false),
        [this]() { return !isAlive(); }, DIE_PRIORITY);
}

// Registers all four states at once, using the shared 6x4 grid and frame
// durations every monster uses.
void Monster::setStandardSpriteAnimations(const std::string& texturePrefix, const std::string& walkTextureKey,
    float attackFrameDuration)
{
    constexpr int SHEET_COLUMNS = 6;
    constexpr int SHEET_ROWS = 4;
    constexpr float WALK_FRAME_DURATION = 0.06f;
    constexpr float IDLE_FRAME_DURATION = 0.08f;
    constexpr float DIE_FRAME_DURATION = 0.05f;

    setWalkAnimation(walkTextureKey, SHEET_COLUMNS, SHEET_ROWS, WALK_FRAME_DURATION);
    setIdleSpriteAnimation(texturePrefix + "_idle", SHEET_COLUMNS, SHEET_ROWS, IDLE_FRAME_DURATION);
    setAttackSpriteAnimation(texturePrefix + "_attack", SHEET_COLUMNS, SHEET_ROWS, attackFrameDuration);
    setDieSpriteAnimation(texturePrefix + "_die", SHEET_COLUMNS, SHEET_ROWS, DIE_FRAME_DURATION);
}

// True while dead but the one-shot Die sheet hasn't finished playing yet.
bool Monster::isDying() const
{
    return !isAlive() && !m_animator.isStateFinished(static_cast<int>(AnimState::Die));
}

// A monster with a Die sheet configured must wait for it to finish before
// Board clears it from its Tile.
bool Monster::isReadyForRemoval() const
{
    if (isAlive()) return false;
    return m_animator.isStateFinished(static_cast<int>(AnimState::Die));
}

void Monster::draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const
{
    if (m_q == -1 && m_row == -1) return;

    if (m_hasTexture)
    {
        // Which state is active was already decided this frame in update()
        // - draw() only reads that decision back, never recomputes it.
        if (m_animator.hasActiveState())
        {
            m_animator.applyCurrentFrame(m_sprite);
        }
        else if (m_animator.hasAnyState())
        {
            // A state is configured but none applies right now - fall back
            // to the static sprite/origin explicitly.
            const sf::Texture& idleTexture = AssetsManager::getInstance().getTexture(m_textureKey);
            m_sprite.setTexture(idleTexture, true); // reset rect back to the full static image
            m_sprite.setOrigin({ idleTexture.getSize().x / 2.f, idleTexture.getSize().y / 2.f });
        }

        // A sheet in use scales by its own base scale, not m_baseScale, so
        // a monster reads as the same on-board size either way.
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
        // Skipped while dying - an actions-left count has no meaning over
        // a monster that's already dead.
		drawHealthBar(window);
        if (m_side ==currentTurnSide)
            drawActionsLeft(window);
    }

    // This monster draws its own in-flight attack animation, if any.
    if (m_attackAnimation)
        m_attackAnimation->draw(window);

    // Same, for an incoming Special Ability effect.
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
    // Clears any freeze in effect - resetActions() runs once per
    // owner-turn-end, exactly when a freeze's blocked turn concludes.
    m_frozen = false;
    m_actionsLeft = 2;
	if (m_specialCooldown > 0)
		m_specialCooldown--;
}

// Zeroes actions immediately so the freeze blocks this monster's very next
// turn; resetActions() clears the flag once that turn ends.
void Monster::applyFreeze()
{
    m_frozen = true;
    m_actionsLeft = 0;
}

void Monster::applyEmpoweredAttack(float multiplier)
{
    m_attackMultiplier = multiplier;
}

void Monster::moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions)
{
    if (m_actionsLeft <= 0 || pathScreenPositions.empty()) return;

    // Logical position updates immediately; the path queue below drives
    // the visual walk through each step in turn.
    m_q = finalQ;
    m_row = finalRow;

    m_pathQueue.assign(pathScreenPositions.begin(), pathScreenPositions.end());
    m_isMoving = true;

    useAction();
}

void Monster::update(float dt)
{
    // Advances independently of movement - must progress even while this
    // monster stands still (attacker stationary, projectile traveling).
    if (m_attackAnimation)
    {
        m_attackAnimation->update(dt);
        if (m_attackAnimation->isFinished())
            m_attackAnimation.reset();
    }

    // Same independence, for an incoming Special effect.
    if (m_specialAnimation)
    {
        m_specialAnimation->update(dt);
        if (m_specialAnimation->isFinished())
            m_specialAnimation.reset();
    }

    // Re-evaluates which registered state (if any) is active this frame -
    // draw() only reads the result back, never recomputes it.
    m_animator.update(dt);

    if (!m_isMoving || m_pathQueue.empty())
    {
        m_isMoving = false;
        return;
    }

    const sf::Vector2f& currentTarget = m_pathQueue.front();
    float dx = currentTarget.x - m_screenPos.x;
    float dy = currentTarget.y - m_screenPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Close enough - snap to this waypoint and advance to the next.
    if (distance < 5.0f)
    {
        m_screenPos = currentTarget;
        m_pathQueue.pop_front();

        if (m_pathQueue.empty())
        {
            m_isMoving = false;
        }
    }
    else
    {
        // Not there yet - step toward the current waypoint.
        float moveX = (dx / distance) * m_speed * dt;
        float moveY = (dy / distance) * m_speed * dt;

        m_screenPos.x += moveX;
        m_screenPos.y += moveY;
    }

    m_sprite.setPosition(m_screenPos);
}

bool Monster::isOnBoard() const {
    return m_q != -1 && m_row != -1;
}

bool Monster::useSpecialAbility(const Board& board, BoardEntity* target)
{
    if (!canUseSpecialAbilityNow())
        return false;

    onSpecialAbility(board, target);

    if (specialAbilityCommitsOnSelect())
    {
        useAction();
        m_specialCooldown = m_baseCooldown;
    }
    // else: onSpecialAbility() only armed monster-specific state - the
    // action/cooldown bookkeeping is deferred until that monster decides
    // its Special has actually been used.

    return true;
}

void Monster::attack(BoardEntity* target)
{
    //const sf::Sound& attackSound = AssetsManager::getInstance().getSound(m_textureKey);
    //attackSound.
    /*SoundPlayer::getInstance().play("attack_hit"); *///מאוחר מדי!!!! לחשוב על מקום אחר לפני שנשים את זה

    // Empowered Attack multiplier consumed here, the one place every
    // attack resolves; 1.f (neutral) when nothing was granted.
    int damage = static_cast<int>(m_attackDamage * m_attackMultiplier);
    m_attackMultiplier = 1.f;

    target->takeDamage(damage);
    useAction();
    if (target->isProtected()) // shield parry sound
        SoundPlayer::getInstance().play("parry_attack");
    if (target->isAlive()) // only play the hit sound if the target survived
        SoundPlayer::getInstance().play("attack_hit");
}

void Monster::playAttackAnimation(std::unique_ptr<AttackAnimation> animation)
{
    //SoundPlayer::getInstance().play("attack_launch"); // נשמע מיד עם תחילת האנימציה!
    m_attackAnimation = std::move(animation);
}

void Monster::playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation)
{
    m_specialAnimation = std::move(animation);
}

// isAlive() explicit: a dying monster is already Tile-linked but must not
// be selectable while its death animation plays.
bool Monster::canBeSelectedBy(PlayerSide side) const
{
    return isAlive() && !isEnemyOf(side) && m_actionsLeft > 0;
}

bool Monster::canBeTargetedBySpecial() const
{
    return true;
}

int Monster::getRange() const
{
    return m_range;
}

// Mirrors attack()'s own damage computation, empowerment included.
int Monster::getAttackDamage() const
{
    return static_cast<int>(m_attackDamage * m_attackMultiplier);
}

int Monster::getActionsLeft() const
{
    return m_actionsLeft;
}

PlayerSide Monster::getSide() const
{
    return m_side;
}

bool Monster::canFly() const
{
    return m_flying;
}

bool Monster::isMoving() const
{
    return m_isMoving;
}

bool Monster::isAttacking() const
{
    return m_attackAnimation != nullptr;
}

bool Monster::isUsingSpecialAnimation() const
{
    return m_specialAnimation != nullptr;
}

int Monster::getSpecialCooldown() const
{
    return m_specialCooldown;
}

bool Monster::isSpecialReady() const
{
    return m_specialCooldown <= 0;
}

// isAlive() matters too: nothing currently clears a dead monster's Card
// link, so without this a dead monster's stale Special could still fire.
bool Monster::canUseSpecialAbilityNow() const
{
    return isAlive() && isSpecialReady() && getActionsLeft() > 0;
}

// Default: every Special currently commits the instant a target is chosen.
bool Monster::specialAbilityCommitsOnSelect() const
{
    return true;
}

// Default no-op: only a Special that arms without committing needs this.
void Monster::cancelSpecialAbility()
{
}

// Default: any alive enemy Monster. Ally-targeted Specials (Heal,
// Protection, Empowered Attack) override this.
bool Monster::isValidSpecialTarget(const BoardEntity& candidate) const
{
    return candidate.isAlive() && candidate.canBeTargetedBySpecial() && candidate.isEnemyOf(m_side);
}

// Default: neutral for every candidate; a monster preferring a specific
// target type overrides this.
float Monster::scoreAsSpecialTarget(const BoardEntity& candidate) const
{
    return 0.f;
}

bool Monster::canMove() const
{
    return isAlive() && m_actionsLeft > 0;
}

void Monster::useAction()
{
    if (m_actionsLeft > 0) m_actionsLeft--;
}
