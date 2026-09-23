#include "State/State.h"
#include "Menu.h"
#include "Constants.h"
#include <functional>

// End-of-match screen: shows the winner and offers Play Again (if a
// rematch is possible) or Main Menu.
class GameOverState : public State
{
public:
    // createNextState may be empty for a PlayerVsRemote match; Play Again is hidden then.
    GameOverState(sf::RenderWindow& window,const PlayerSide& winner,
        std::function<std::unique_ptr<State>()> createNextState);

    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void handleEvent(const sf::Event& event) override;

private:
    void scaleBackgroundToWindow();
    void buildMenu();
    void onMainMenuClicked();
    void onRestartClicked();
    void setupWinnerText(const PlayerSide& winner);

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Menu m_menu; 
    sf::Text m_winnerText;
    std::function<std::unique_ptr<State>()> m_createNextState;
};