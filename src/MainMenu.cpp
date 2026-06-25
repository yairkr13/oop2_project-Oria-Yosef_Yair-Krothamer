#include "MainMenu.h"
#include "TextureManager.h"
#include <stdexcept>
#include <iostream> // TEMP DEBUG

// Main Menu button areas in IMAGE-SPACE coordinates (1672x941)
static constexpr float IMG_START_X = 525.f;
static constexpr float IMG_START_Y = 420.f;
static constexpr float IMG_START_R = 1150.f;
static constexpr float IMG_START_B = 570.f;

static constexpr float IMG_INSTR_X = 525.f;
static constexpr float IMG_INSTR_Y = 585.f;
static constexpr float IMG_INSTR_R = 1150.f;
static constexpr float IMG_INSTR_B = 730.f;

static constexpr float IMG_EXIT_X = 525.f;
static constexpr float IMG_EXIT_Y = 745.f;
static constexpr float IMG_EXIT_R = 1150.f;
static constexpr float IMG_EXIT_B = 890.f;

// Back button area in IMAGE-SPACE coordinates (Instructions.png)
static constexpr float IMG_BACK_X = 28.f;
static constexpr float IMG_BACK_Y = 20.f;
static constexpr float IMG_BACK_R = 85.f;
static constexpr float IMG_BACK_B = 135.f;

// Window size
static constexpr float WINDOW_W = 1280.f;
static constexpr float WINDOW_H = 720.f;

// Helper: convert image-space rect to screen-space FloatRect
static sf::FloatRect imageToScreen(float left, float top, float right, float bottom,
    float imgW, float imgH)
{
    float sx = WINDOW_W / imgW;
    float sy = WINDOW_H / imgH;
    return sf::FloatRect({ left * sx, top * sy }, { (right - left) * sx, (bottom - top) * sy });
}

MainMenu::MainMenu(sf::RenderWindow& window)
    : m_window(window)
    , m_activeIndex(0)
    , m_showInstructions(false)
    , m_instrObjective(m_font)
    , m_instrControls(m_font)
    , m_instrDismiss(m_font)
    , m_title(m_font)
	, m_bgSprite(TextureManager::getInstance().get<sf::Texture>("menu_bg")),
	m_instrSprite(TextureManager::getInstance().get<sf::Texture>("instructions_bg"))
    /*, m_bgSprite([]() -> const sf::Texture& {
    TextureManager::getInstance().loadTexture("menu_bg", "resources/Menu/Menu.png");
    return TextureManager::getInstance().getTexture("menu_bg");
        }())
    , m_instrSprite([]() -> const sf::Texture& {
    TextureManager::getInstance().loadTexture("instructions_bg", "resources/Menu/Instructions.png");
    return TextureManager::getInstance().getTexture("instructions_bg");
        }())*/
{
    loadAssets();
    buildItems();
}

void MainMenu::reset()
{
    m_activeIndex = 0;
    m_showInstructions = false;
    updateHighlight();
}

std::optional<MenuResult> MainMenu::handleEvent(const sf::Event& event)
{
    if (m_showInstructions)
    {
        if (handleInstructionsEvent(event))
            m_showInstructions = false;
        return std::nullopt;
    }
    return handleMenuEvent(event);
}

void MainMenu::draw()
{
    m_window.setView(m_window.getDefaultView());
    if (m_showInstructions)
        drawInstructions();
    else
        drawMenu();
}

void MainMenu::loadAssets()
{
    // Scale menu background to fit window (1280x720)
    {
        auto texSize = m_bgSprite.getTexture().getSize();
        float scaleX = 1280.f / static_cast<float>(texSize.x);
        float scaleY = 720.f / static_cast<float>(texSize.y);
        m_bgSprite.setScale({ scaleX, scaleY });
    }

    // Scale instructions background to fit window
    {
        auto texSize = m_instrSprite.getTexture().getSize();
        float scaleX = 1280.f / static_cast<float>(texSize.x);
        float scaleY = 720.f / static_cast<float>(texSize.y);
        m_instrSprite.setScale({ scaleX, scaleY });
    }

    //// Load font
    //if (!m_font.openFromFile("resources/arial.ttf"))
    //    throw std::runtime_error(
    //        "MainMenu: failed to load font 'resources/arial.ttf'");
}

void MainMenu::buildItems()
{
    m_items.clear();

    // Get actual menu image size for scaling
    auto texSize = m_bgSprite.getTexture().getSize();
    float imgW = static_cast<float>(texSize.x);
    float imgH = static_cast<float>(texSize.y);

    // Convert image-space button rects to screen-space
    m_clickZones.clear();
    m_clickZones.push_back(imageToScreen(IMG_START_X, IMG_START_Y, IMG_START_R, IMG_START_B, imgW, imgH));
    m_clickZones.push_back(imageToScreen(IMG_INSTR_X, IMG_INSTR_Y, IMG_INSTR_R, IMG_INSTR_B, imgW, imgH));
    m_clickZones.push_back(imageToScreen(IMG_EXIT_X, IMG_EXIT_Y, IMG_EXIT_R, IMG_EXIT_B, imgW, imgH));
}

void MainMenu::updateHighlight()
{
    // No visible text to highlight — buttons are in the image
}

std::optional<MenuResult> MainMenu::handleMenuEvent(const sf::Event& event)
{
    const std::size_t N = m_clickZones.size();

    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            m_activeIndex = (m_activeIndex + N - 1) % N;
            break;

        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            m_activeIndex = (m_activeIndex + 1) % N;
            break;

        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:
            return activateCurrentItem();

        case sf::Keyboard::Key::Escape:
            return MenuResult::Quit;

        default:
            break;
        }
    }
    else if (const auto* mm = event.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2f pos(static_cast<float>(mm->position.x),
            static_cast<float>(mm->position.y));
        for (std::size_t i = 0; i < m_clickZones.size(); ++i)
        {
            if (m_clickZones[i].contains(pos))
            {
                m_activeIndex = i;
                break;
            }
        }
    }
    else if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f pos(static_cast<float>(mb->position.x),
                static_cast<float>(mb->position.y));
            std::cout << "[Menu] Mouse clicked at: x=" << pos.x << ", y=" << pos.y << std::endl; // TEMP DEBUG
            for (std::size_t i = 0; i < m_clickZones.size(); ++i)
            {
                if (m_clickZones[i].contains(pos))
                {
                    m_activeIndex = i;
                    return activateCurrentItem();
                }
            }
        }
    }

    return std::nullopt;
}

bool MainMenu::handleInstructionsEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape ||
            key->code == sf::Keyboard::Key::Backspace)
            return true;
    }
    else if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f pos(static_cast<float>(mb->position.x),
                static_cast<float>(mb->position.y));
            std::cout << "[Instructions] Mouse clicked at: x=" << pos.x << ", y=" << pos.y << std::endl; // TEMP DEBUG
            // Back button in SCREEN-SPACE (measured from debug clicks)
            static constexpr float BACK_SCR_X = 25.f;
            static constexpr float BACK_SCR_Y = 15.f;
            static constexpr float BACK_SCR_W = 115.f;  // 140 - 25
            static constexpr float BACK_SCR_H = 80.f;   // 95 - 15
            sf::FloatRect backZone({ BACK_SCR_X, BACK_SCR_Y }, { BACK_SCR_W, BACK_SCR_H });
            if (backZone.contains(pos))
                return true;
        }
    }
    return false;
}

std::optional<MenuResult> MainMenu::activateCurrentItem()
{
    switch (m_activeIndex)
    {
    case 0:  return MenuResult::Play;
    case 1:  m_showInstructions = true; return std::nullopt;
    case 2:  return MenuResult::Quit;
    default: return std::nullopt;
    }
}

void MainMenu::drawMenu()
{
    // Draw background image only — buttons are part of the image
    m_window.draw(m_bgSprite);
}

void MainMenu::drawInstructions()
{
    // Draw Instructions.png as full background
    m_window.draw(m_instrSprite);
}

