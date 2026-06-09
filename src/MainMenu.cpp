#include "MainMenu.h"
#include <stdexcept>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
MainMenu::MainMenu(sf::RenderWindow& window)
    : m_window(window)
    , m_activeIndex(0)
    , m_showInstructions(false)
    , m_instrObjective(m_font)
    , m_instrControls(m_font)
    , m_instrDismiss(m_font)
    , m_title(m_font)
{
    loadAssets();  // throws std::runtime_error if font is missing
    buildItems();
}

// ---------------------------------------------------------------------------
// reset() — call whenever the menu becomes visible again
// ---------------------------------------------------------------------------
void MainMenu::reset()
{
    m_activeIndex      = 0;
    m_showInstructions = false;
    updateHighlight();
}

// ---------------------------------------------------------------------------
// handleEvent() — feed one event, returns a result when player decides
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// draw() — render the current screen into the window (no clear / display)
// ---------------------------------------------------------------------------
void MainMenu::draw()
{
    m_window.setView(m_window.getDefaultView());
    if (m_showInstructions)
        drawInstructions();
    else
        drawMenu();
}

// ---------------------------------------------------------------------------
// Asset loading
// ---------------------------------------------------------------------------
void MainMenu::loadAssets()
{
    if (!m_font.openFromFile("resources/arial.ttf"))
        throw std::runtime_error(
            "MainMenu: failed to load font 'resources/arial.ttf'");

    // Title
    m_title = sf::Text(m_font, "Parking Mania", TITLE_SIZE);
    m_title.setFillColor(COLOR_TITLE);
    {
        const float tw = m_title.getLocalBounds().size.x;
        m_title.setPosition({ (800.f - tw) / 2.f, 60.f });
    }

    // Instructions screen texts
    m_instrObjective = sf::Text(m_font,
        "Objective: Park your car in the marked spots without hitting obstacles.",
        24u);
    m_instrObjective.setFillColor(COLOR_TITLE);
    {
        const float tw = m_instrObjective.getLocalBounds().size.x;
        m_instrObjective.setPosition({ (800.f - tw) / 2.f, 180.f });
    }

    m_instrControls = sf::Text(m_font,
        "Arrow keys / WASD to drive.   Escape to return to menu.",
        24u);
    m_instrControls.setFillColor(COLOR_TITLE);
    {
        const float tw = m_instrControls.getLocalBounds().size.x;
        m_instrControls.setPosition({ (800.f - tw) / 2.f, 260.f });
    }

    m_instrDismiss = sf::Text(m_font,
        "Press Escape or Backspace to go back.",
        24u);
    m_instrDismiss.setFillColor({ 180, 180, 180, 255 });
    {
        const float tw = m_instrDismiss.getLocalBounds().size.x;
        m_instrDismiss.setPosition({ (800.f - tw) / 2.f, 380.f });
    }
}

// ---------------------------------------------------------------------------
// Build the three menu items
// ---------------------------------------------------------------------------
void MainMenu::buildItems()
{
    struct LabelPos { const char* label; };
    const LabelPos entries[] = { {"Play"}, {"Instructions"}, {"Exit"} };

    m_items.clear();
    m_items.reserve(3);

    for (std::size_t i = 0; i < 3; ++i)
    {
        sf::Text text(m_font, entries[i].label, ITEM_SIZE);
        const float tw = text.getLocalBounds().size.x;
        text.setPosition({ (800.f - tw) / 2.f,
                           ITEMS_TOP + static_cast<float>(i) * ITEM_SPACING });

        m_items.push_back({ std::string(entries[i].label), std::move(text) });
    }

    updateHighlight();
}

// ---------------------------------------------------------------------------
// Highlight
// ---------------------------------------------------------------------------
void MainMenu::updateHighlight()
{
    for (std::size_t i = 0; i < m_items.size(); ++i)
        m_items[i].text.setFillColor(i == m_activeIndex ? COLOR_ACTIVE : COLOR_INACTIVE);
}

// ---------------------------------------------------------------------------
// Event handling — main menu
// ---------------------------------------------------------------------------
std::optional<MenuResult> MainMenu::handleMenuEvent(const sf::Event& event)
{
    const std::size_t N = m_items.size();

    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            m_activeIndex = (m_activeIndex + N - 1) % N;
            updateHighlight();
            break;

        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            m_activeIndex = (m_activeIndex + 1) % N;
            updateHighlight();
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
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].text.getGlobalBounds().contains(pos))
            {
                m_activeIndex = i;
                updateHighlight();
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
            for (std::size_t i = 0; i < m_items.size(); ++i)
            {
                if (m_items[i].text.getGlobalBounds().contains(pos))
                {
                    m_activeIndex = i;
                    return activateCurrentItem();
                }
            }
        }
    }

    return std::nullopt;
}

// ---------------------------------------------------------------------------
// Event handling — instructions screen
// ---------------------------------------------------------------------------
bool MainMenu::handleInstructionsEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape ||
            key->code == sf::Keyboard::Key::Backspace)
            return true;  // dismiss
    }
    return false;
}

// ---------------------------------------------------------------------------
// Activate the currently highlighted item
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------
void MainMenu::drawMenu()
{
    m_window.draw(m_title);
    for (const auto& item : m_items)
        m_window.draw(item.text);
}

void MainMenu::drawInstructions()
{
    m_window.draw(m_instrObjective);
    m_window.draw(m_instrControls);
    m_window.draw(m_instrDismiss);
}
