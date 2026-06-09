#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

// Returned by MainMenu::handleEvent() to tell the caller what to do next.
enum class MenuResult { Play, Quit, Closed };

// One selectable entry in the menu.
struct MenuItem
{
    std::string label;
    sf::Text    text;
};

class MainMenu
{
public:
    /// @throws std::runtime_error if resources/arial.ttf cannot be loaded.
    explicit MainMenu(sf::RenderWindow& window);

    /// Reset highlight and state — call each time the menu becomes visible.
    void reset();

    /// Feed one SFML event. Returns a MenuResult when the player makes a
    /// decisive choice; returns nullopt for all other events.
    std::optional<MenuResult> handleEvent(const sf::Event& event);

    /// Draw the current menu / instructions frame (does NOT call display()).
    void draw();

private:
    sf::RenderWindow&      m_window;
    sf::Font               m_font;
    sf::Text               m_title;
    std::vector<MenuItem>  m_items;          // [0]=Play [1]=Instructions [2]=Exit
    std::size_t            m_activeIndex;
    bool                   m_showInstructions;

    // Instruction-screen texts (built once, reused every frame)
    sf::Text m_instrObjective;
    sf::Text m_instrControls;
    sf::Text m_instrDismiss;

    // Visual constants
    static constexpr unsigned  TITLE_SIZE      = 56u;
    static constexpr unsigned  ITEM_SIZE       = 30u;
    static constexpr float     ITEM_SPACING    = 60.f;
    static constexpr float     ITEMS_TOP       = 250.f;
    static constexpr sf::Color COLOR_ACTIVE    = { 255, 220,  50, 255 };  // gold
    static constexpr sf::Color COLOR_INACTIVE  = { 200, 200, 200, 255 };  // grey
    static constexpr sf::Color COLOR_TITLE     = { 255, 255, 255, 255 };  // white

    void loadAssets();
    void buildItems();

    std::optional<MenuResult> handleMenuEvent(const sf::Event& event);
    bool                      handleInstructionsEvent(const sf::Event& event);
    std::optional<MenuResult> activateCurrentItem();

    void updateHighlight();
    void drawMenu();
    void drawInstructions();
};
