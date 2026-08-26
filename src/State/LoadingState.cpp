#include "State/LoadingState.h"
#include "State/MenuState.h"
#include "SpriteUtils.h"
#include "AssetsManager.h"
#include <algorithm>

namespace
{
    // Degrees per second the spinner rotates at - fast enough to clearly
    // read as spinning during the brief time this state is ever on screen.
    // Applied as speed * deltaTime.asSeconds() in update() below, not as a
    // fixed per-call amount, so the angle actually turned each frame stays
    // proportional to real elapsed time no matter how long that frame's
    // own loading work (see LOAD_TIME_BUDGET below) took.
    constexpr float SPINNER_ROTATION_SPEED = 240.f;

    // On-screen size (max dimension, in pixels) the spinner is drawn at -
    // Spinner.png's own native resolution (1278x1230) is irrelevant here;
    // see the uniform-scale computation in the constructor below, which
    // derives the actual scale from the real loaded texture size against
    // this constant, the same "size independent of native resolution"
    // approach SpinningProjectileAnimation already uses for the muffin
    // projectile.
    constexpr float SPINNER_DISPLAY_SIZE = 72.f;

    // Upper bound on how much loading work update() does in a single
    // frame: keeps calling AssetsManager::loadNext() (see below) for as
    // long as the assets it's loading are cheap, instead of spending a
    // whole separate frame on each one - most of the ~60 queued assets
    // are small (button/UI icons, fonts) and would otherwise each waste a
    // full draw for near-zero work. Checked only *between* individual
    // loadNext() calls, never during one, so this changes nothing about
    // how long any single asset's own sf::Texture::loadFromFile() call
    // takes - in particular the ~20 monster sprite sheets in the queue,
    // each a ~5-11MB PNG decoding to roughly 88MB of raw pixels
    // (5760x3840 RGBA), still take however long that one blocking call
    // takes and still show up as one slower frame. That residual pause is
    // an inherent limit of loading textures synchronously on the main
    // thread - genuinely smoothing it away would mean decoding those
    // specific assets on a background thread, deliberately out of scope
    // here (see LoadingState.h).
    const sf::Time LOAD_TIME_BUDGET = sf::milliseconds(4);
}

LoadingState::LoadingState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("await_bg"))
    , m_spinner(AssetsManager::getInstance().getTexture("spinner"))
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());

    // Origin at the spinner's own center, not its default top-left corner,
    // so rotate() below spins it in place instead of orbiting it around a
    // corner. Positioned at the window's center - the empty area of
    // AwaitScreen.png.
    sf::Vector2u spinnerSize = m_spinner.getTexture().getSize();
    m_spinner.setOrigin({ spinnerSize.x / 2.f, spinnerSize.y / 2.f });
    m_spinner.setPosition({ static_cast<float>(m_window.getSize().x) / 2.f,
                             static_cast<float>(m_window.getSize().y) / 2.f });

    // Uniform scale (aspect ratio preserved) sized against
    // SPINNER_DISPLAY_SIZE rather than Spinner.png's real pixel size
    // (1278x1230) - without this the sprite drew at native resolution,
    // nearly covering the whole 1280x720 window.
    float maxSpinnerDim = static_cast<float>(std::max(spinnerSize.x, spinnerSize.y));
    float spinnerScale = (maxSpinnerDim > 0.f) ? (SPINNER_DISPLAY_SIZE / maxSpinnerDim) : 1.f;
    m_spinner.setScale({ spinnerScale, spinnerScale });

    // Everything except this state's own two assets (already loaded by
    // Controller before this state existed - see AssetsManager::
    // loadBootAssets()) is queued here rather than loaded now; update()
    // below drains that queue incrementally, across frames.
    AssetsManager::getInstance().queueRemainingAssets();
}

void LoadingState::update(sf::Time deltaTime)
{
    m_spinner.rotate(sf::degrees(SPINNER_ROTATION_SPEED * deltaTime.asSeconds()));

    // Loads queued assets one at a time (see AssetsManager::loadNext()),
    // repeating within this single frame only while under
    // LOAD_TIME_BUDGET - batches multiple cheap assets together so this
    // state doesn't burn a full frame on each of them, while never
    // touching how long any one asset's own load takes (see the comment
    // on LOAD_TIME_BUDGET above). Guaranteed to call loadNext() at least
    // once even if the budget is already exceeded on entry, so this state
    // always makes forward progress every frame.
    sf::Clock frameLoadClock;
    bool hasMore;
    do
    {
        hasMore = AssetsManager::getInstance().loadNext();
    } while (hasMore && frameLoadClock.getElapsedTime() < LOAD_TIME_BUDGET);

    if (!hasMore)
        transitionTo(std::make_unique<MenuState>(m_window));
}

void LoadingState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    window.draw(m_spinner);
}

void LoadingState::handleEvent(const sf::Event& event)
{
    // Nothing to interact with while loading.
}
