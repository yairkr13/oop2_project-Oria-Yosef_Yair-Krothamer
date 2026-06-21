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
    explicit MainMenu(sf::RenderWindow& window);

    void reset();
    std::optional<MenuResult> handleEvent(const sf::Event& event);
    void draw();

private:
    sf::RenderWindow& m_window;
    sf::Font               m_font;
    sf::Text               m_title;
    sf::Sprite             m_bgSprite;
    sf::Sprite             m_instrSprite;
    std::vector<sf::FloatRect> m_clickZones; // invisible click areas matching image buttons
    std::vector<MenuItem>  m_items;
    std::size_t            m_activeIndex;
    bool                   m_showInstructions;

    // Instruction-screen texts
    sf::Text m_instrObjective;
    sf::Text m_instrControls;
    sf::Text m_instrDismiss;

    // Visual constants
    static constexpr unsigned  TITLE_SIZE = 56u;
    static constexpr unsigned  ITEM_SIZE = 30u;
    static constexpr float     ITEM_SPACING = 60.f;
    static constexpr float     ITEMS_TOP = 310.f;
    static constexpr sf::Color COLOR_ACTIVE = { 255, 220,  50, 255 };
    static constexpr sf::Color COLOR_INACTIVE = { 200, 200, 200, 255 };
    static constexpr sf::Color COLOR_TITLE = { 255, 255, 255, 255 };

    void loadAssets();
    void buildItems();

    std::optional<MenuResult> handleMenuEvent(const sf::Event& event);
    bool                      handleInstructionsEvent(const sf::Event& event);
    std::optional<MenuResult> activateCurrentItem();

    void updateHighlight();
    void drawMenu();
    void drawInstructions();
};
