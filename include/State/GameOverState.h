#include "State/State.h"
#include "Menu.h"
#include "Constants.h"
#include <functional>

class GameOverState : public State
{
public:
    // createNextState may be an empty std::function - GameplayState passes
    // an empty one for a PlayerVsRemote match (there's no way to redo the
    // host/join handshake in-place), and "Play Again" simply isn't shown.
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